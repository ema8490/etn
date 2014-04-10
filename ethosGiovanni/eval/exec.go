package ev

import (
	"bytes"
	"ethos/elTypes"
	"ethos/environment"
	"ethos/ethos"
	"ethos/io"
	"ethos/nodetypes"
	"ethos/parser"
	esys "ethos/syscall"
	"fmt"
	"strings"
	"syscall"
)

var verbose = false

func pipelinesDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	i := 0
	l := len(node.Children)

	for i < l {
		pipeline := node.Children[i]

		ppEnv := env.Copy(env.Name + "/pipeline")
		Eval(pipeline, ppEnv)

		panik := ppEnv.IsSetPanic()
		e, o := ppEnv.GetPanic()

		if i < l-1 {
			expr := node.Children[i+1]

			if !panik && expr.Type == nt.OR_EXPRESSION {
				break
			}

			if panik && expr.Type == nt.AND_EXPRESSION {
				env.SetPanic(true, e, o)
				break
			}

			if panik && expr.Type != nt.OR_EXPRESSION {
				env.SetPanic(true, e, o)
			}

		} else if panik {
			env.SetPanic(true, e, o)
		}

		node.ActualType, node.ActualValue = pipeline.ActualType, pipeline.ActualValue

		i += 2
	}

	return true
}

func pipelineDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {

	wfd := esys.Fd(esys.Stdout)
	status := esys.Status(esys.StatusOk)
	var nextHashIn []byte

	wfds := make([]esys.Fd, len(node.Children)-1)
	pids := make([]int, len(node.Children))

	if len(node.Children) == 1 {
		// check for an overriding function definition
		f := node.Children[0]
		name := string(f.Children[0].Value)
		if builtin[name] != nil || env.Symbols.Lookup(name) != nil {
			// drop redirections
			if len(f.Children) > 1 {
				f.Children = f.Children[:len(f.Children)-1]
			}
			functionCall(f, env, env)
			node.ActualValue, node.ActualType = f.ActualValue, f.ActualType
			return true
		}
	}

	for i := len(node.Children) - 1; i >= 0; i-- {
		wfd, pids[i], nextHashIn, status = exec(node.Children[i], env, wfd, nextHashIn, i > 0)
		if status != esys.StatusOk {
			env.SetPanic(true, nil, env)
			return true
		}
		if i > 0 {
			wfds[i-1] = wfd
		}
	}

	if node.Type == nt.PIPELINE {
		for i, pid := range pids {
			status, exit := waitPid(pid)
			check(fmt.Sprintf("waitPid %d", pid), status)
			if exit != 0 {
				env.SetPanic(true, &parser.ParseTree{ActualType: t.STRING, ActualValue: fmt.Sprintf("\"%s\" exited with status %d", node.Children[i].Children[0].Value, exit)}, env)
				return true
			}
			if i > 0 {
				check(fmt.Sprintf("closeFd %d", wfds[i-1]), esys.Close(wfds[i-1]))
			}
		}
	}

	return true
}

var pipeid = 0

func exec(node *parser.ParseTree, env *en.StandardEnvironment, prevWfd esys.Fd, nextHashIn []byte, pipe bool) (wfd esys.Fd, pid int, hashIn []byte, status esys.Status) {
	defer func() {
		r := recover()
		if r != nil {
			fmt.Println(r)
		}
	}()

	// build path
	name := string(node.Children[0].Value)
	path := io.MakePath(name, env.WD, true)
	if path == "" {
		fmt.Printf("Unknown command \"%s\"", string(node.Children[0].Value))
		return esys.Stdout, 0, nil, esys.Status(1)
	}

	// build redirs
	var redirOutFd, redirInFd esys.Fd
	redirOut := false
	redirIn := false
	var redirInHash []byte
	redirs := node.Children[len(node.Children)-1]
	for _, redir := range redirs.Children {
		path := string(redir.Value)
		switch redir.Type {
		case nt.REDIR_OUT:
			redirOutFd, status = ethos.OpenDirectoryPath(path)
			check(fmt.Sprintf("Open redir out: %s", path), status)
			nextHashIn = io.GetDirHash(path)
			redirOut = true
		case nt.REDIR_IN:
			redirInFd, status = ethos.OpenDirectoryPath(path)
			check(fmt.Sprintf("Open redir in: %s", path), status)
			redirInHash = io.GetDirHash(path)
			redirIn = true
		}
	}

	// type check
	hashIn = io.GetDirHash(path + "-in")
	hashOut := io.GetDirHash(path + "-out")

	if nextHashIn == nil && !bytes.Equal(hashOut, []byte{}) {
		fmt.Printf("Cannot pipe %s (out:\"%s\") to untyped stdout\n", name, io.GetName(hashOut))
		return esys.Stdout, 0, hashIn, esys.Status(1)
	}

	if !bytes.Equal(hashOut, nextHashIn) {
		fmt.Printf("Cannot pipe %s (out:\"%s\") into \"%s\"\n", name, io.GetName(hashOut), io.GetName(nextHashIn))
		return esys.Stdout, 0, hashIn, esys.Status(1)
	}

	if !pipe && !redirIn && !bytes.Equal(hashIn, []byte{}) {
		fmt.Printf("%s (in:\"%s\") cannot accept untyped stdin\n", name, io.GetName(hashIn))
		return esys.Stdout, 0, hashIn, esys.Status(1)
	}

	if redirIn && !bytes.Equal(hashIn, redirInHash) {
		fmt.Printf("%s (in:\"%s\") cannot accept \"%s\" stdin\n", name, io.GetName(hashIn), io.GetName(redirInHash))
		return esys.Stdout, 0, hashIn, esys.Status(1)
	}

	// build args
	args := []string{}
	for _, child := range node.Children[1 : len(node.Children)-1] {
		if child.Type == nt.IDENTIFIER {
			Eval(child, env)
			args = append(args, fmt.Sprintf("%v", child.ActualValue))
		} else {
			args = append(args, string(child.Value))
		}
	}

	// make pipe if needed
	var rfd esys.Fd
	if pipe {
		wfd, rfd, status = makePipe(hashIn)
		check("makePipe", status)
	} else {
		rfd = esys.Stdin
	}
	// redirs have precedence over pipe
	if redirIn {
		if rfd != esys.Stdin {
			check(fmt.Sprintf("Redirout: Close unused rfd(%d)", rfd), esys.Close(rfd))
		}
		rfd = redirInFd
	}
	if redirOut {
		if prevWfd != esys.Stdout {
			check(fmt.Sprintf("Redirout: Close unused prevWfd(%d)", prevWfd), esys.Close(prevWfd))
		}
		prevWfd = redirOutFd
	}

	// fork
	parentPid := esys.GetPid()
	terminalFd, _, status := esys.Fork(0)
	check("Fork", status)

	if esys.GetPid() != parentPid {
		// copy, close & exec
		if pipe || redirIn {
			check(fmt.Sprintf("Close Stdin(%d)", esys.Stdin), esys.Close(esys.Stdin))
			check(fmt.Sprintf("CopyFd Stdin(%d), rfd(%d)", esys.Stdin, rfd), esys.CopyFd(esys.Stdin, rfd))
			check(fmt.Sprintf("Close rfd(%d)", rfd), esys.Close(rfd))
		}
		if prevWfd != esys.Stdout {
			check(fmt.Sprintf("Close Stdout(%d)", esys.Stdout), esys.Close(esys.Stdout))
			check(fmt.Sprintf("CopyFd Stdout(%d), prevWfd(%d)", esys.Stdout, prevWfd), esys.CopyFd(esys.Stdout, prevWfd))
			check(fmt.Sprintf("Close prevWfd(%d)", prevWfd), esys.Close(prevWfd))
		}

		syscall.Exec(path, args)
	}

	// cleanup parent
	if pipe {
		check(fmt.Sprintf("Close rfd(%d)", rfd), esys.Close(rfd))
	}

	return wfd, int(terminalFd), hashIn, status
}

func makePipe(hash []byte) (wfd, rfd esys.Fd, status esys.Status) {
	defer func() {
		r := recover()
		if r != nil {
			fmt.Println(r)
		}
	}()

refresh:
	//TODO: random pipeid
	pipeid++
	serviceDir := fmt.Sprintf("/programs/pipe-%d", pipeid)
	serviceName := fmt.Sprintf("pipe-%d", pipeid)

	rmFd, status := ethos.OpenDirectoryPath(serviceDir)
	if status == esys.StatusOk {
		esys.Close(rmFd)

		parts := strings.Split(serviceDir, "/")
		pipeDir := strings.Join(parts[:len(parts)-1], "/")
		pipeName := parts[len(parts)-1]

		rmFd, status = ethos.OpenDirectoryPath(pipeDir)
		check("makePipe OpenDir rm", status)

		status = ethos.RemoveDirectory(rmFd, pipeName)
		check("makePipe RemoveDir", status)

		esys.Close(rmFd)
	}

	status = ethos.CreateDirectoryPath(serviceDir, "", hash)
	check("makePipe CreateDir", status)

	serviceFd, status := ethos.OpenDirectoryPath(serviceDir)
	defer esys.Close(serviceFd)
	check("makePipe OpenDir", status)

	// try to advertise
	listeningFd, status := ethos.Advertise(serviceFd, serviceName)
	if status != esys.StatusOk {
		goto refresh
	}
	defer esys.Close(listeningFd)

	// actually build the pipe
	evtImport, status := esys.Import(listeningFd)
	check("makePipe Import evt", status)

	evtIpc, status := esys.Ipc(serviceFd, serviceName, "", nil)
	check("makePipe Ipc evt", status)

	_, rfd, status = esys.BlockAndRetire(evtImport)
	check("makePipe Import", status)

	_, wfd, status = esys.BlockAndRetire(evtIpc)
	check("makePipe Ipc", status)

	return wfd, rfd, status
}

func check(str string, status esys.Status) {
	if status != esys.StatusOk {
		panic(fmt.Sprintf("[ERR] %s, status: %d\n", str, status))
	} else if verbose {
		fmt.Println(str)
	}
}

func waitPid(pid int) (esys.Status, int) {
	evt, status := esys.Read(esys.Fd(pid))
	if status != esys.StatusOk {
		return status, 0
	}
	b, _, status := esys.BlockAndRetire(evt)
	if status != esys.StatusOk {
		return status, 0
	}
	e := uint32(b[0]) | uint32(b[1])<<8 | uint32(b[2])<<16 | uint32(b[3])<<24
	return status, int(e)
}
