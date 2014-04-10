package ev

import (
	"ethos/elTypes"
	"ethos/environment"
	"ethos/nodetypes"
	"ethos/operators"
	"ethos/parser"
	"ethos/util"
	"reflect"
	// "fmt"
)

func ifThenElseDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	condition := node.Children[0]
	thenBlock := node.Children[1]

	Eval(condition, env)
	c, _ := condition.ActualValue.(bool)

	if c {
		Eval(thenBlock, env)
	} else if len(node.Children) > 2 {
		Eval(node.Children[2], env)
	}

	return true
}

func switchDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	expression := node.Children[0]
	cases := node.Children[1:]

	Eval(expression, env)
	for _, caseB := range cases {
		if caseB.Type != nt.CASE_ELSE {
			caseExpression := caseB.Children[0]
			Eval(caseExpression, env)
			if caseExpression.ActualValue == expression.ActualValue {
				Eval(caseB.Children[1], env)
				break
			}
		} else {
			Eval(caseB.Children[0], env)
			break
		}
	}

	return true
}

func forDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	init := node.Children[0]
	condition := node.Children[1]
	step := node.Children[2]
	block := node.Children[3]

	forEnv := env.Copy(env.Name + "/for")
	Eval(init, forEnv)

	// init condition value
	Eval(condition, forEnv)
	c, _ := condition.ActualValue.(bool)

	forEnv.SetInLoop(true)

	// cycle
	for c {
		Eval(block, forEnv)
		if forEnv.IsSetPanic() {
			HandlePanic(forEnv)
			break
		}
		if forEnv.IsSetBreak() {
			break
		}
		if forEnv.IsSetContinue() {
			forEnv.SetContinue(false)
		}
		if forEnv.IsSetReturn() {
			ret, t := forEnv.GetReturn()
			env.SetReturn(true, ret, t)
			break
		}
		Eval(step, forEnv)
		Eval(condition, forEnv)
		c, _ = condition.ActualValue.(bool)
	}

	return true
}

func forEachDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	idCouple := node.Children[0]
	var id, k *parser.ParseTree
	if len(idCouple.Children) == 1 {
		id = idCouple.Children[0]
	} else {
		k = idCouple.Children[0]
		id = idCouple.Children[1]
	}
	rangeId := node.Children[1]
	block := node.Children[2]

	forEnv := env.Copy(env.Name + "/for")
	Eval(id, forEnv)
	if k != nil {
		Eval(k, forEnv)
	}
	Eval(rangeId, forEnv)

	forEnv.SetInLoop(true)
	forEnv.Symbols.Put(id.ActualId, nil, id.ActualType)
	symbol := forEnv.Symbols.Lookup(id.ActualId)
	var symbolK *en.Symbol
	if k != nil {
		forEnv.Symbols.Put(k.ActualId, nil, k.ActualType)
		symbolK = forEnv.Symbols.Lookup(k.ActualId)
	}
	for index := 0; true; index += 1 {
		// get current or break
		breakLoop := false
		switch rangeId.ActualType {
		case t.STRING:
			if index >= stringLength(rangeId) {
				breakLoop = true
			} else {
				symbol.Value, symbol.Type = accessString(rangeId, index)
				if k != nil {
					symbolK.Value = index
				}
			}
		case t.TUPLE:
			if index >= tupleLength(rangeId) {
				breakLoop = true
			} else {
				symbol.Value, symbol.Type = accessTuple(rangeId, index)
				if k != nil {
					symbolK.Value = index
				}
			}
		case t.MAP:
			next := mapNextIndex(rangeId, index)
			if next == "" {
				breakLoop = true
			} else {
				ind := new(parser.ParseTree)
				ind.ActualValue = next
				ind.ActualType = t.STRING
				symbol.Value, symbol.Type = accessMap(rangeId, ind)
				if k != nil {
					symbolK.Value = next
					symbolK.Type = t.STRING
				}
			}
		default:
			op.UnexpectedType(rangeId)
			breakLoop = true
		}
		if breakLoop {
			break
		}

		// eval body
		Eval(block, forEnv)

		if forEnv.IsSetPanic() {
			HandlePanic(forEnv)
			break
		}
		if forEnv.IsSetBreak() {
			break
		}
		if forEnv.IsSetContinue() {
			forEnv.SetContinue(false)
		}
		if forEnv.IsSetReturn() {
			ret, t := forEnv.GetReturn()
			env.SetReturn(true, ret, t)
			break
		}
	}

	return true
}

func blockDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	for _, statement := range node.Children {
		Eval(statement, env)
		if env.IsSetBreak() || env.IsSetContinue() || env.IsSetReturn() || env.IsSetPanic() {
			break
		}
	}
	return true
}

func functionDefinitionDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	name := node.Children[0]

	// create lexical scope and closure
	value := new(FunctionSymbolValue)
	value.signature = node
	value.env = env

	// func name()
	if name != nil {
		Eval(name, env)
		id := name.ActualId

		// push or update reference
		symbol := env.Symbols.Lookup(id)
		if symbol != nil {
			symbol.Value = value
		} else {
			env.Symbols.Put(id, value, t.FUNCTION)
		}

		// propagate id
		node.ActualId = id
	} else {
		// func()
		node.ActualValue = value
	}

	node.ActualType = t.FUNCTION

	return true
}

func functionCallDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	return functionCall(node, env, env)
}

func valueAccessDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	value := node.Children[0]
	access := node.Children[1]

	Eval(value, env)
	switch value.ActualValue.(type) {
	case *RequireAccess:
		return pkgAccess(node, value, access, env)
	case reflect.Value:
		return accessStruct(node, value, access, env)
	default:
		util.Printf("Invalid value access\n")
	}
	return true
}

func pkgAccess(node, value, access *parser.ParseTree, env *en.StandardEnvironment) bool {
	// check if pkg has been required
	accessor, ok := value.ActualValue.(*RequireAccess)
	if !ok {
		op.UnexpectedType(value)
	}

	if accessor == nil {
		return true
	}

	// pkg needs evaluation before access
	if !accessor.Evaluated {
		Eval(accessor.Root, accessor.Env)
		accessor.Evaluated = true
	}

	switch access.Type {
	case nt.FUNCTION_CALL:
		functionCall(access, accessor.Env, env)
	case nt.IDENTIFIER:
		identifier(access, accessor.Env, env)
	default:
		// TODO
	}

	node.ActualValue, node.ActualType = access.ActualValue, access.ActualType

	return true
}

func squareAccessDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	value := node.Children[0]
	access := node.Children[1]

	Eval(value, env)
	Eval(access, env)

	switch value.ActualType {
	case t.STRING:
		index, _ := access.ActualValue.(int)
		node.ActualValue, node.ActualType = accessString(value, index)
	case t.TUPLE:
		index, _ := access.ActualValue.(int)
		node.ActualValue, node.ActualType = accessTuple(value, index)
	case t.MAP:
		node.ActualValue, node.ActualType = accessMap(value, access)
	case t.OBJECT:
		index, _ := access.ActualValue.(int)
		node.ActualValue, node.ActualType = accessSlice(value, index)
	default:
		op.UnexpectedType(value)
	}

	return true
}

func assignmentDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	// stop only if destination is an access
	// (simpler assignments are handled upwards)
	destination := node.Children[0]
	if destination.Type != nt.SQUARE_ACCESS {
		return false
	}

	source := node.Children[1].Copy()
	value := destination.Children[0]
	access := destination.Children[1]
	Eval(value, env)
	Eval(access, env)
	Eval(source, env)

	symbol := env.Symbols.Lookup(value.ActualId)
	if symbol == nil {
		return true
	}

	switch value.ActualType {
	case t.STRING:
		symbol.Value = setString(value, source, access)
		break
	case t.TUPLE:
		symbol.Value = setTuple(value, source, access)
		break
	case t.MAP:
		symbol.Value = setMap(value, source, access)
		break
	default:
		op.UnexpectedType(value)
	}

	return true
}
