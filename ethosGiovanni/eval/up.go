package ev

import (
	"ethos/elTypes"
	"ethos/environment"
	"ethos/io"
	"ethos/nodetypes"
	"ethos/operators"
	"ethos/parser"
	"strconv"
)

func addUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.SUM, node.Children[0], node.Children[1])
	return false
}

func subUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.SUB, node.Children[0], node.Children[1])
	return false
}

func mulUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.MUL, node.Children[0], node.Children[1])
	return false
}

func divUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.DIV, node.Children[0], node.Children[1])
	return false
}

func unaryMinusUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.UNARY_MINUS, node.Children[0])
	return false
}

func boolLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualType = t.BOOL
	if string(node.Value) == "true" {
		node.ActualValue = true
	} else {
		node.ActualValue = false
	}
	return false
}

func stringLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualType = t.STRING
	node.ActualValue = string(node.Value)
	return false
}

func intLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, _ = strconv.Atoi(string(node.Value))
	node.ActualType = t.INT
	return false
}

func floatLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, _ = strconv.Atof64(string(node.Value))
	node.ActualType = t.FLOAT
	return false
}

func tupleLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = newTuple(node.Children...)
	return false
}

func mapLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = newMap(node.Children...)
	return false
}

func pathLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	path := io.MakePath(string(node.Value), env.WD, false)
	if path == "" {
		node.ActualType = t.UNDEFINED
		node.ActualValue = nil
		return false
	}

	if io.IsDirectory(path) {
		node.ActualValue, node.ActualType = newMapFromDir(path)
	} else {
		if io.IsPrimitive(io.GetTypeName(path)) {
			node.ActualValue, node.ActualType = io.ReadPrimitive(path)
		} else {
			node.ActualValue = io.ReadObject(path)
			if node.ActualValue != nil {
				node.ActualType = t.OBJECT
			}
		}
	}
	return false
}

func setLiteralUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = newSet(node.Children...)
	return false
}

func expressionUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue = node.Children[0].ActualValue
	return false
}

func identifierUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	return identifier(node, env, env)
}

func assignmentUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	source := node.Children[1]
	value := source.ActualValue
	dest := node.Children[0]

	if dest.Type == nt.PATH_LITERAL {
		path := string(dest.Value)
		if io.IsDirectory(path) {
			dirAssignment(path, value)
		} else {
			if source.ActualType == t.OBJECT {
				io.WriteObject(value, path)
			} else {
				io.WritePrimitive(value, path)
			}
		}
		return false
	}

	id := dest.ActualId
	at := source.ActualType

	symbol := env.Symbols.Lookup(id)
	if symbol == nil {
		env.Symbols.Put(id, value, at)
	} else {
		if symbol.Type == t.MAP {
			m, _ := symbol.Value.(*t.MapRep)
			if m.Source != "" {
				// destination map is backed by a directory
				dirAssignment(m.Source, value)
				return false
			}
		}
		symbol.Value = value
		symbol.Type = at
	}

	return false
}

func eComparisonUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.EQUAL, node.Children[0], node.Children[1])
	return false
}

func nEComparisonUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.NOT_EQUAL, node.Children[0], node.Children[1])
	return false
}

func lComparisonUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.LOWER, node.Children[0], node.Children[1])
	return false
}

func gComparisonUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.GREATER, node.Children[0], node.Children[1])
	return false
}

func lEComparisonUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.LOWER_EQUAL, node.Children[0], node.Children[1])
	return false
}

func gEComparisonUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.GREATER_EQUAL, node.Children[0], node.Children[1])
	return false
}

func notUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.NOT, node.Children[0])
	return false
}

func andExpressionUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.AND, node.Children[0], node.Children[1])
	return false
}

func orExpressionUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue, node.ActualType = op.Apply(op.OR, node.Children[0], node.Children[1])
	return false
}

func forConditionUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	node.ActualValue = node.Children[0].ActualValue
	return false
}

func breakUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	if env.IsInLoop() {
		env.SetBreak(true)
	}

	return false
}

func continueUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	if env.IsInLoop() {
		env.SetContinue(true)
	}

	return false
}

func exportUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	symbol := env.Symbols.Lookup(node.Children[0].ActualId)
	symbol.Exported = true

	return false
}

func varUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	id := node.Children[0].ActualId

	if len(node.Children) == 2 {
		env.Symbols.Put(id, node.Children[1].ActualValue, node.Children[1].ActualType)
	} else {
		env.Symbols.Put(id, nil, t.UNDEFINED)
	}
	// TODO: multiple declarations with same id

	// propagate id
	node.ActualId = id

	return false
}

func returnUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	if len(node.Children) == 1 {
		env.SetReturn(true, node.Children[0].ActualValue, node.Children[0].ActualType)
	} else {
		env.SetReturn(true, nil, 0)
	}
	return false
}
