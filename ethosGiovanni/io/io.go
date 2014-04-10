package io

import (
	"ethos/elScript"
	"ethos/elTypes"
	"ethos/ethos"
	"ethos/parser"
	esys "ethos/syscall"
	"ethos/typeDefs"
	"ethos/util"
	"path"
	"strings"
	"syscall"
)

func InitWD() string {
	me := esys.GetUser()
	return "/user/" + me + "/scripts"
}

func TestWD() string {
	me := esys.GetUser()
	return "/user/" + me + "/tests"
}

func Exists(path string) bool {
	var st syscall.Stat_t
	return syscall.Stat(path, &st) == 0
}

func MakePathExists(p string, wd string, cmd bool) (string, bool) {
	p = MakePath(p, wd, cmd)
	return p, Exists(p)
}

func MakePath(p string, wd string, cmd bool) string {
	var ap string
	var sp []string

	if path.IsAbs(p) {
		sp = []string{""}
	} else {
		sp = []string{wd}
		if cmd {
			sp = append(sp, "/programs")
		}
	}

	for _, s := range sp {
		ap = path.Join(s, p)
		if Exists(ap) {
			return ap
		}
	}

	return ap
}

func MakeNewPath(p string, wd string, cmd bool) string {
	parts := strings.Split(p, "/")
	if len(parts) > 1 {
		parent := MakePath(strings.Join(parts[0:len(parts)-1], "/"), wd, cmd)
		if parent != "" {
			return parent + "/" + parts[len(parts)-1]
		}

	}

	return MakePath(p, wd, cmd)
}

func MakeNewPathExists(p string, wd string, cmd bool) (string, bool) {
	p = MakeNewPath(p, wd, cmd)
	parts := strings.Split(p, "/")
	return p, Exists(strings.Join(parts[:len(parts)-1], "/"))
}

func ReadScript(input string) string {
	return readScriptFile(input, InitWD())
}

func ReadTestScript(input string) string {
	return readScriptFile(input, TestWD())
}

func readScriptFile(input string, wd string) string {
	// open scripts dir
	dirFd, status := ethos.OpenDirectoryPath(wd)
	if status != esys.StatusOk {
		util.Fatalf("Error calling OpenDirectory %s\n", wd)
	}
	defer esys.Close(dirFd)

	_, status = ethos.GetFileInformationPath(wd + "/" + input)
	if status != esys.StatusOk {
		util.Printf("not found: %s/%s\n", wd, input)
		return ""
	}

	// read script file
	reader := ethos.NewVarReader(dirFd, input)
	decoder := elScript.NewDecoder(reader)
	file, err := decoder.ElScript()
	if err != nil {
		util.Printf(err.String())
		return ""
	}

	return file.Content
}

func WriteScript(input string, content string) {
	writeScriptFile(input, content, InitWD())
}

func WriteTest(input string, content string) {
	writeScriptFile(input, content, TestWD())
}

func writeScriptFile(input string, content string, path string) {
	dirFd, status := ethos.OpenDirectoryPath(path)
	if status != esys.StatusOk {
		util.Fatalf("Error opening %s\n", path)
	}
	defer esys.Close(dirFd)

	writer := ethos.NewVarWriter(dirFd, input)
	encoder := elScript.NewEncoder(writer)
	script := new(elScript.ElScript)
	script.Content = content
	encoder.ElScript(script)
	encoder.Flush()
	util.Printf("wrote to: %s/%s\n", path, input)
}

func ReadTypedDir(path string) ([]string, []interface{}) {
	ret := []interface{}{}
	retNames := []string{}

	path = MakePath(path, "", false)
	dirFd, status := ethos.OpenDirectoryPath(path)
	if status != esys.StatusOk {
		return retNames, ret
	}

	hash := GetDirHash(InitWD() + "/" + path)
	kind := t.TypeFromEtnHash(hash)

	lastName := ""
	for bytes, status := ethos.GetNextName(dirFd, lastName); status == esys.StatusOk; bytes, status = ethos.GetNextName(dirFd, lastName) {
		if status == esys.StatusNotFound {
			break
		}

		lastName = string(bytes)

		if lastName == "." || lastName == ".." || lastName == "" {
			continue
		}

		reader := ethos.NewVarReader(dirFd, lastName)
		decoder := typeDefs.NewDecoder(reader)

		var file, err interface{}

		if !t.IsComposite(kind) {
			file, err = ReadPrimitive(lastName)
		} else {
			file, err = readComposite(kind, decoder)
		}

		if file == nil || err != nil {
			util.Fatalf("Unknown type %s\n", t.TYPES[kind])
		}
		ret = append(ret, file)
		retNames = append(retNames, lastName)
	}
	return retNames, ret
}

func emptyDir(dirFd esys.Fd) {
	lastName := ""
	for bytes, status := ethos.GetNextName(dirFd, lastName); status == esys.StatusOk; bytes, status = ethos.GetNextName(dirFd, lastName) {
		if status == esys.StatusNotFound {
			break
		}

		lastName = string(bytes)

		if lastName == "." || lastName == ".." || lastName == "" {
			continue
		}

		ethos.RemoveFile(dirFd, lastName)
	}
}

func WriteTypedDir(path string, m *t.MapRep) bool {
	kind := m.Type
	hash := GetHash(t.ETN_NAMES[kind])

	path = MakePath(path, "", false)
	dirFd := accessOrCreate(path, hash)

	if dirFd == 0 {
		util.Printf("Wrong type for destination directory %s\n", path)
		return false
	}

	emptyDir(dirFd)

	for k, e := range m.Content {
		element, _ := e.(*parser.ParseTree)
		writer := ethos.NewVarWriter(dirFd, k)
		encoder := typeDefs.NewEncoder(writer)

		if !t.IsComposite(kind) {
			WritePrimitive(element.ActualValue, k)
		} else {
			writeComposite(element.ActualValue, kind, encoder)
		}
	}
	return false
}

func ListDirectory(path string) []string {
	retNames := []string{}
	path = MakePath(path, "", false)
	dirFd, status := ethos.OpenDirectoryPath(path)
	if status != esys.StatusOk {
		return retNames
	}

	lastName := ""
	for bytes, status := ethos.GetNextName(dirFd, lastName); status == esys.StatusOk; bytes, status = ethos.GetNextName(dirFd, lastName) {
		if status == esys.StatusNotFound {
			break
		}

		lastName = string(bytes)

		if lastName == "." || lastName == ".." || lastName == "" {
			continue
		}

		retNames = append(retNames, lastName)
	}
	return retNames
}

func IsDirectory(path string) bool {
	_, status := ethos.OpenDirectoryPath(path)

	return status == esys.StatusOk
}

// FIXME: copy-pasted from goPackages/ethos/sync.go
func openPathLastDirectory(p string) (dirFd esys.Fd, fileName string, status esys.Status) {
	var start esys.Fd
	if path.IsAbs(p) {
		start = esys.RootDirectory
	} else {
		start = esys.CurrentDirectory
	}

	nodes := strings.Split(p, "/")

	if len(nodes) == 0 { // i.e., ""
		dirFd, fileName, status = 0, "", esys.StatusInvalidFileName
	} else { // i.e., /foo/bar...
		dirFd = start
		parent := dirFd

		for _, n := range nodes[:len(nodes)-1] {
			if n == "" {
				continue
			}
			dirFd, status = ethos.OpenDirectory(parent, n)
			if status != esys.StatusOk {
				return 0, "", status
			}

			if parent != esys.RootDirectory && parent != esys.CurrentDirectory {
				esys.Close(parent)
			}

			parent = dirFd
		}

		fileName = nodes[len(nodes)-1]
	}

	return dirFd, fileName, status
}
