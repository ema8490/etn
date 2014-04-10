/*
	Builtin functions are the 'std' package	for El.

	Builtin functions have the signature of a generic Evaluator, that is: they receive as input the
	node (root of a ParseTree) and the execution
	Environment, and have to take care of decorating
	the received sub-tree and modify the Env
	accordingly (other than	accomplishing their task).
*/
package ev

import (
	"ethos/elTypes"
	"ethos/environment"
	"ethos/io"
	"ethos/nodetypes"
	"ethos/operators"
	"ethos/parser"
	"ethos/util"
	"fmt"
	"syscall"
)

type RequireAccess struct {
	Root      *parser.ParseTree
	Env       *en.StandardEnvironment
	Evaluated bool
}

func print(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	formatString, _ := node.Children[1].ActualValue.(string)
	params := node.Children[2:]
	values := []interface{}{}
	for _, param := range params {
		values = append(values, param.ActualValue)
	}
	util.Printf(formatString+"\n", values...)

	return true
}

func scan(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	s := ""
	fmt.Scanln(&s)
	node.ActualValue = s
	node.ActualType = t.STRING

	return true
}

func sprint(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	formatString, _ := node.Children[1].ActualValue.(string)
	params := node.Children[2:]
	values := []interface{}{}
	for _, param := range params {
		values = append(values, param.ActualValue)
	}
	node.ActualValue = fmt.Sprintf(formatString, values...)
	node.ActualType = t.STRING

	return true
}

func require(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	path, _ := node.Children[1].ActualValue.(string)

	content := io.ReadScript(path)

	// new environment
	rAccess := new(RequireAccess)
	rAccess.Env = new(en.StandardEnvironment)
	rAccess.Env.Init(env.WD, "main/"+path)

	// parse input
	lexer := new(parser.Lexer)
	lexer.Init(string(content))
	rAccess.Root = parser.Parse(lexer)

	node.ActualValue, node.ActualType = rAccess, t.PACKAGE

	return true
}

func kind(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	value := node.Children[1]
	node.ActualValue = op.CompositeTypeName(op.CompositeType(value))
	node.ActualType = t.STRING

	return true
}

func length(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	value, kind := node.Children[1].ActualValue, node.Children[1].ActualType
	node.ActualType = t.INT
	switch kind {
	case t.STRING:
		v, _ := value.(string)
		node.ActualValue = len(v)
		return true
	case t.TUPLE:
		v, _ := value.(*t.TupleRep)
		node.ActualValue = v.Len
		return true
	case t.OBJECT:
		l := lenSlice(value)
		if l > 0 {
			node.ActualValue = l
			return true
		}

	}

	op.UnexpectedType(node.Children[1])
	node.ActualType = t.UNDEFINED

	return true
}

func concat(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	a, b, kind := node.Children[1], node.Children[2], node.Children[1].ActualType

	if kind != t.TUPLE {
		op.UnexpectedType(a)
		node.ActualType = t.UNDEFINED
		return true
	}
	aTuple, ok1 := a.ActualValue.(*t.TupleRep)
	bTuple, ok2 := b.ActualValue.(*t.TupleRep)
	if !ok1 {
		op.UnexpectedType(a)
		node.ActualType = t.UNDEFINED
		return true
	}
	if !ok2 {
		op.UnexpectedType(b)
		node.ActualType = t.UNDEFINED
		return true
	}
	if aTuple.Len > 0 && bTuple.Len > 0 {
		if op.CompositeType(a) != op.CompositeType(b) {
			op.UnexpectedType(b)
			node.ActualType = t.UNDEFINED
			return true
		}
	}
	resultType := t.UNDEFINED
	if aTuple.Type != t.UNDEFINED {
		resultType = aTuple.Type
	}
	if bTuple.Type != t.UNDEFINED {
		resultType = bTuple.Type
	}

	newTuple := &t.TupleRep{Content: []interface{}{}, Type: resultType, Len: aTuple.Len + bTuple.Len}

	newTuple.Content = append(newTuple.Content, aTuple.Content...)
	newTuple.Content = append(newTuple.Content, bTuple.Content...)

	node.ActualValue = newTuple
	node.ActualType = kind

	return true
}

func postpend(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	a, b, kind := node.Children[1], node.Children[2].Copy(), node.Children[1].ActualType

	if kind != t.TUPLE {
		op.UnexpectedType(a)
		node.ActualType = t.UNDEFINED
		return true
	}
	aTuple, _ := a.ActualValue.(*t.TupleRep)
	if aTuple.Type != t.UNDEFINED {
		if aTuple.Len > 0 {
			el, _ := aTuple.Content[0].(*parser.ParseTree)
			if op.CompositeType(el) != op.CompositeType(b) {
				op.UnexpectedType(b)
				node.ActualType = t.UNDEFINED
				return true
			}
		} else if aTuple.Type != b.Type {
			op.UnexpectedType(b)
			node.ActualType = t.UNDEFINED
			return true
		}
	}
	aTuple.Content = append(aTuple.Content, b)
	aTuple.Len++
	aTuple.Type = b.ActualType

	node.ActualValue = aTuple
	node.ActualType = t.TUPLE

	return true
}

func panik(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	if len(node.Children) > 1 {
		env.SetPanic(true, node.Children[1], env)
	} else {
		env.SetPanic(true, &parser.ParseTree{}, env)
	}

	return true
}

func try(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	tEnv := env.Copy("try")

	if len(node.Children) > 2 {
		handler := new(parser.ParseTree)
		handler.Type = nt.FUNCTION_CALL
		handler.Children = []*parser.ParseTree{node.Children[2]}
		tEnv.SetHandler(&PanicHandler{handler})
	} else {
		tEnv.SetHandler(&PanicHandler{nil})
	}

	wrapped := new(parser.ParseTree)
	wrapped.Type = nt.FUNCTION_CALL
	wrapped.Children = []*parser.ParseTree{node.Children[1]}

	Eval(wrapped, tEnv)
	if tEnv.IsSetPanic() {
		HandlePanic(tEnv)
	}

	return true
}

func assert(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	if len(node.Children) != 3 {
		util.Printf("Assert requires 2 parameters. %d given", len(node.Children))
		return true
	}
	valid, ok := node.Children[1].ActualValue.(bool)
	if !ok {
		original, oko := node.Children[2].ActualValue.(string)
		msg := new(parser.ParseTree)
		msg.ActualType = t.STRING
		assertMsg := "non-bool given as assert parameter"
		msg.ActualValue = assertMsg
		if oko {
			msg.ActualValue = assertMsg + ":" + original
		}
		env.SetPanic(true, msg, env)
	}
	if !valid {
		env.SetPanic(true, node.Children[2].Copy(), env)
	}

	return true
}

func cd(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	dest := io.MakePath(string(node.Children[1].Value), env.WD, false)
	if dest == "" {
		util.Printf("No such directory")
		return true
	}
	if !io.IsDirectory(dest) {
		util.Printf("Not a directory")
		return true
	}

	if syscall.Chdir(dest) != 0 {
		util.Printf("Error Chdir")
		return true
	}
	env.WD = dest
	return true
}
