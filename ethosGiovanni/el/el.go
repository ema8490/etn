package main

import (
	"bufio"
	"ethos/elTypes"
	"ethos/environment"
	"ethos/eval"
	"ethos/io"
	"ethos/nodetypes"
	"ethos/operators"
	"ethos/parser"
	"ethos/util"
	"flag"
	"fmt"
	"os"
	"regexp"
	"strings"
)

const (
	PROMPT_STRING       = "el > "
	EXIT_STRING         = "exit"
	CLEAN_STRING        = "clean"
	TABLE_STRING        = "table"
	LOAD_RE_STRING      = `load[ ]+(.+)`
	SAVE_RE_STRING      = `save[ ]+(.+)`
	SAVE_TEST_RE_STRING = `savetest[ ]+(.+)`
	ENV_NAME            = "main/REPL"
)

var env *en.StandardEnvironment

func newEnv(name string) *en.StandardEnvironment {
	env := new(en.StandardEnvironment)
	env.Init(io.InitWD(), name)
	return env
}

func showTable(env *en.StandardEnvironment) {
	fmt.Printf("Symbols:\n")
	for _, symbol := range env.Symbols.List() {
		if t.IsComposite(symbol.Type) {
			node := new(parser.ParseTree)
			node.ActualValue = symbol.Value
			node.ActualType = symbol.Type
			ct := op.CompositeType(node)
			fmt.Printf(" - %s : %v [%s]\n", symbol.Id, symbol.Value, op.CompositeTypeName(ct))
		} else {
			fmt.Printf(" - %s : %v [%s]\n", symbol.Id, symbol.Value, t.TYPES[symbol.Type])
		}
	}
	fmt.Printf("\n")
}

var completionStrings = []string{
	EXIT_STRING,
	CLEAN_STRING,
	TABLE_STRING,
	"load ",
	"save ",
	"savetest ",
}

func completion(input string) []string {
	res := []string{}
	for _, s := range completionStrings {
		if m, _ := regexp.Match("^"+input, []byte(s)); m {
			res = append(res, s)
		}
	}
	return res
}

func repl() {
	loadRE := regexp.MustCompile(LOAD_RE_STRING)
	saveRE := regexp.MustCompile(SAVE_RE_STRING)
	saveTestRE := regexp.MustCompile(SAVE_TEST_RE_STRING)

	input := []string{}
	reader := bufio.NewReader(os.Stdin)
	for {
		fmt.Printf("\n" + PROMPT_STRING)

		byteLine, _, err := reader.ReadLine()
		if err != nil {
			fmt.Printf(err.String())
			continue
		}
		line := string(byteLine)

		if line == EXIT_STRING {
			// exit repl
			break
		}

		if line == CLEAN_STRING {
			// clean environment and input up to now
			env = newEnv(ENV_NAME)
			input = []string{}
			continue
		}

		if line == TABLE_STRING {
			// show symbol table
			showTable(env)
			continue
		}

		matched := saveTestRE.FindStringSubmatch(line)
		if matched != nil {
			// save test
			content := strings.Join(input, "\n")
			io.WriteTest(matched[len(matched)-1], content)
			continue
		}

		matched = saveRE.FindStringSubmatch(line)
		if matched != nil {
			// save file
			content := strings.Join(input, "\n")
			io.WriteScript(matched[len(matched)-1], content)
			continue
		}

		matched = loadRE.FindStringSubmatch(line)
		if matched != nil {
			// clean env, load file, change wd
			file := matched[len(matched)-1]
			parts := strings.Split(file, "/")
			wd := strings.Join(parts[:len(parts)-1], "/")
			env.WD = wd
			line = io.ReadScript(file)
		}

		// evaluate current input
		parsed := eval(line)
		if parsed != "" {
			input = append(input, parsed)
		}
	}
}

func _parse(input string) (root *parser.ParseTree, parsed string, ok bool) {
	lexer := new(parser.Lexer)
	lexer.Init(input)
	var out *parser.ParseTree
	ok = true
	defer func() {
		err := recover()
		if err != nil {
			msg, _ := err.(string)
			fmt.Printf(msg)
			ok = false
		}
	}()
	out = parser.Parse(lexer)
	return out, input, ok
}

func parse(input string) (root *parser.ParseTree, parsed string) {
	root, parsed, _ = _parse(input)
	return
}

func eval(input string) string {
	root, parsed := parse(input)
	ev.Eval(root, env)
	if env.IsSetPanic() {
		ev.HandlePanic(env)
	}
	return parsed
}

func printTree(input string) {
	root, _ := parse(input)
	root.Walk(0, printNode, none, nil)
}

func printNode(level int, node *parser.ParseTree, e interface{}) bool {
	for i := 0; i < level; i += 1 {
		fmt.Printf("|  ")
	}
	fmt.Printf("%s [%s]\n", nt.NODE_TYPES[node.Type], node.Value)
	return false
}

func none(level int, node *parser.ParseTree, e interface{}) bool { return false }

func runTests(name string) {
	names := io.ListDirectory(io.TestWD())
	failed := []string{}
	if name != "all" {
		filtered := []string{}
		for _, n := range names {
			if strings.Contains(n, name) {
				filtered = append(filtered, n)
			}
		}
		names = filtered
	}
	var tEnv *en.StandardEnvironment
	for _, test := range names {
		input := io.ReadTestScript(test)
		root, _, ok := _parse(input)
		if !ok {
			failed = append(failed, test)
			fmt.Printf("FAILED: %s\n", test)
			continue
		} else {
			tEnv = env.Copy("test")
			ev.Eval(root, tEnv)
			if tEnv.IsSetPanic() {
				panik, _ := tEnv.GetPanic()
				node, _ := panik.(*parser.ParseTree)
				msg, okk := node.ActualValue.(string)
				if okk {
					fmt.Printf("FAILED: %s - %s\n", test, msg)
				} else {
					fmt.Printf("FAILED: %s\n", test)
				}
				failed = append(failed, test)
				continue
			}
		}

		fmt.Printf("OK    : %s\n", test)
	}

	fmt.Printf("\n%d tests failing\n", len(failed))
	for _, f := range failed {
		fmt.Printf("%s	", f)
	}
	fmt.Println("")
}

func main() {
	io.RegisterTypes()

	inputFile := flag.String("input", "", "file to be evaluated")
	tree := flag.Bool("tree", false, "print parse tree")
	test := flag.String("test", "", "test case to run. Use 'all' to run all tests")
	verbose := flag.Bool("verbose", false, "run tests reporting el output")
	flag.Parse()

	env = newEnv(ENV_NAME)
	ev.Init()

	defer func() {
		if r := recover(); r != nil {
			fmt.Println("\n[panic] ", r, "\n")
			repl()
		}
	}()

	if *inputFile != "" {
		input := io.ReadScript(*inputFile)
		if *tree {
			printTree(input)
		} else {
			eval(input)
		}
		return
	}

	if *test != "" {
		if !*verbose {
			util.SetMode(util.TEST)
		}
		runTests(*test)
		return
	}

	repl()
}
