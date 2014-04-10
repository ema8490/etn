/*
	This package provides the core evaluation
	functionalities.

	Evaluation happens directly on the ParseTree
	result of the parsing phase and is structured
	as an attribute grammar where attributes are
	passed decorating directly the tree nodes.
	Both synthesized and inherited attributes are
	needed: this is obtained defining two
	(disjunct) sets of evaluators: downEvaluators
	are executed on the nodes during the descending
	phase, upEvaluators during the ascending phase.
	Evaluators have the ability to control the
	traversing of the tree (stopping evaluation of
	a sub-tree) as defined by the Walker interface (type) provided by ParseTree itself (see el/parser/parsetree.go).
*/
package ev

import (
	"ethos/elTypes"
	"ethos/environment"
	"ethos/nodetypes"
	"ethos/operators"
	"ethos/parser"
	"ethos/util"
	"fmt"
)

type Evaluator func(node *parser.ParseTree, env *en.StandardEnvironment) bool

/*
	Evaluates the sub-tree for which the
	given node is root, within the given
	Environment.
*/
func Eval(node *parser.ParseTree, env *en.StandardEnvironment) {
	node.Walk(0, walkerDown, walkerUp, env)
}

var downEvaluators = map[int]Evaluator{}

var upEvaluators = map[int]Evaluator{}

var builtin = map[string]Evaluator{}

func walkerDown(level int, node *parser.ParseTree, env interface{}) bool {
	e, _ := env.(*en.StandardEnvironment)
	return evalDown(node, e)
}

func walkerUp(level int, node *parser.ParseTree, env interface{}) bool {
	e, _ := env.(*en.StandardEnvironment)
	return evalUp(node, e)
}

func evalDown(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	if downEvaluators[node.Type] != nil {
		return downEvaluators[node.Type](node, env)
	}
	return false
}

func evalUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	if upEvaluators[node.Type] != nil {
		return upEvaluators[node.Type](node, env)
	}
	return false
}

func Init() {
	op.Init()

	downEvaluators = map[int]Evaluator{
		nt.IFTHEN:              ifThenElseDown,
		nt.IFTHENELSE:          ifThenElseDown,
		nt.SWITCH:              switchDown,
		nt.FOR:                 forDown,
		nt.FOREACH:             forEachDown,
		nt.FUNCTION_DEFINITION: functionDefinitionDown,
		nt.FUNCTION_CALL:       functionCallDown,
		nt.BLOCK:               blockDown,
		nt.VALUE_ACCESS:        valueAccessDown,
		nt.SQUARE_ACCESS:       squareAccessDown,
		nt.ASSIGNMENT:          assignmentDown,
		nt.PIPELINE:            pipelineDown,
		nt.PIPELINES:           pipelinesDown,
		nt.PIPELINE_BG:         pipelineDown,
	}

	upEvaluators = map[int]Evaluator{
		nt.STRING_LITERAL: stringLiteralUp,
		nt.INT_LITERAL:    intLiteralUp,
		nt.FLOAT_LITERAL:  floatLiteralUp,
		nt.BOOL_LITERAL:   boolLiteralUp,
		nt.TUPLE_LITERAL:  tupleLiteralUp,
		nt.MAP_LITERAL:    mapLiteralUp,
		nt.SET_LITERAL:    setLiteralUp,
		nt.EXPRESSION:     expressionUp,
		nt.ADD:            addUp,
		nt.SUB:            subUp,
		nt.MUL:            mulUp,
		nt.DIV:            divUp,
		nt.IDENTIFIER:     identifierUp,
		nt.ASSIGNMENT:     assignmentUp,
		nt.E_COMPARISON:   eComparisonUp,
		nt.L_COMPARISON:   lComparisonUp,
		nt.G_COMPARISON:   gComparisonUp,
		nt.L_E_COMPARISON: lEComparisonUp,
		nt.G_E_COMPARISON: gEComparisonUp,
		nt.N_E_COMPARISON: nEComparisonUp,
		nt.NOT:            notUp,
		nt.OR_EXPRESSION:  orExpressionUp,
		nt.AND_EXPRESSION: andExpressionUp,
		nt.FOR_CONDITION:  forConditionUp,
		nt.RETURN:         returnUp,
		nt.BREAK:          breakUp,
		nt.CONTINUE:       continueUp,
		nt.EXPORT:         exportUp,
		nt.VAR:            varUp,
		nt.UNARY_MINUS:    unaryMinusUp,
		nt.PATH_LITERAL:   pathLiteralUp,
		nt.CONSTRUCTOR:    constructorUp,
	}

	builtin = map[string]Evaluator{
		"print":   print,
		"scan":    scan,
		"sprint":  sprint,
		"require": require,
		"type":    kind,
		"len":     length,
		"concat":  concat,
		"append":  postpend,
		"panic":   panik,
		"try":     try,
		"assert":  assert,
		"cd":      cd,
	}
}

func identifier(node *parser.ParseTree, sigEnv, actualEnv *en.StandardEnvironment) bool {
	node.ActualId = string(node.Value)
	symbol := sigEnv.Symbols.Lookup(node.ActualId)
	if symbol != nil {
		if sigEnv == actualEnv || symbol.Exported {
			node.ActualValue = symbol.Value
			node.ActualType = symbol.Type
		} else if sigEnv != actualEnv {
			unexported(node, symbol)
		}
	}
	return false
}

type FunctionSymbolValue struct {
	signature *parser.ParseTree
	env       *en.StandardEnvironment
}

func functionCall(node *parser.ParseTree, sigEnv, actualEnv *en.StandardEnvironment) bool {

	// eval id and actual params
	identifier := node.Children[0]
	Eval(identifier, sigEnv)
	id := identifier.ActualId
	actualParams := node.Children[1:]
	for _, actualParam := range actualParams {
		Eval(actualParam, actualEnv)
	}

	// check builtins first
	if builtin[id] != nil {
		builtin[id](node, actualEnv)
		return true
	}

	// identifier could be a function definition
	signature := identifier
	fEnv := actualEnv.Copy("closure")
	// otherwise get signature
	if signature.Type != nt.FUNCTION_DEFINITION {
		ok := false
		signature, fEnv, ok = lookupFunction(node, signature, sigEnv, actualEnv)
		if !ok {
			return true
		}
	}

	// push actual params
	sigParams := signature.Children[1:]
	sigParams = sigParams[:len(sigParams)-1]

	actualParamsLength := len(actualParams)
	for i, sigParam := range sigParams {
		Eval(sigParam, fEnv)
		if i < actualParamsLength {
			fEnv.Symbols.Put(sigParam.ActualId, actualParams[i].ActualValue, actualParams[i].ActualType)
		} else {
			fEnv.Symbols.Put(sigParam.ActualId, nil, t.UNDEFINED)
		}
	}

	// eval function body
	fEnv.SetInLoop(false)
	fEnv.CallerEnv = actualEnv
	Eval(signature.Children[len(signature.Children)-1], fEnv)

	// handle return condition
	if fEnv.IsSetPanic() {
		HandlePanic(fEnv)
	} else if fEnv.IsSetReturn() {
		node.ActualValue, node.ActualType = fEnv.GetReturn()
	}

	return true
}

func lookupFunction(node, signature *parser.ParseTree, sigEnv, actualEnv *en.StandardEnvironment) (*parser.ParseTree, *en.StandardEnvironment, bool) {
	identifier := node.Children[0]
	var s *FunctionSymbolValue
	ok := false
	// from value
	s, ok = signature.ActualValue.(*FunctionSymbolValue)
	if !ok {
		// or from symbol
		symbol := sigEnv.Symbols.Lookup(identifier.ActualId)
		if symbol == nil || symbol.Value == nil {
			// actualEnv.SetPanic(true, NewPanic(_undefined(identifier)), actualEnv)
			undefined(identifier)
			return nil, nil, false
		}
		s, ok = symbol.Value.(*FunctionSymbolValue)
		if !ok {
			op.UnexpectedType(symbol.Value)
			return nil, nil, false
		}

		// if environments differ, symbol has to be exported
		if sigEnv != actualEnv && !symbol.Exported {
			unexported(node, symbol)
			return nil, nil, false
		}
	}
	return s.signature, s.env.Copy(identifier.ActualId), true
}

type PanicHandler struct {
	Handler *parser.ParseTree
}

func HandlePanic(env *en.StandardEnvironment) {
	panik, origin := env.GetPanic()
	if env.IsSetHandler() {
		h, _ := env.GetHandler().(*PanicHandler)
		// stop panic-ing
		env.SetPanic(false, nil, nil)
		if h.Handler != nil {
			// evaluate handler, passing err as first param
			p, ok := panik.(*parser.ParseTree)
			if ok {
				h.Handler.Children = append(h.Handler.Children, p)
			}
			Eval(h.Handler, env)
		}
	} else if env.CallerEnv != nil {
		// pop panic to caller, if any
		env.CallerEnv.SetPanic(true, panik, origin)
	} else {
		util.Printf("unhandled panic: %v\n", panicValue(panik))
		o, _ := origin.(*en.StandardEnvironment)
		for caller := o; caller != nil; {
			util.Printf(" in: %s\n", caller.Name)
			caller = caller.CallerEnv
		}
		env.SetPanic(false, nil, nil)
	}
}

func NewPanic(err string) *parser.ParseTree {
	return &parser.ParseTree{ActualType: t.STRING, ActualValue: err}
}

func panicValue(p interface{}) interface{} {
	node, ok := p.(*parser.ParseTree)
	if ok && node.ActualValue != nil {
		return node.ActualValue
	}
	return "unspecified error"
}

func undefined(node *parser.ParseTree) {
	id := node.ActualId
	if id == "" {
		id = string(node.Value)
	}
	util.Printf("Undefined '%s' at line %d\n", id, node.Position.StartLine)
}

func _undefined(node *parser.ParseTree) string {
	id := node.ActualId
	if id == "" {
		id = string(node.Value)
	}
	return fmt.Sprintf("Undefined '%s' at line %d\n", id, node.Position.StartLine)
}

func unexported(node *parser.ParseTree, symbol *en.Symbol) {
	util.Printf("Unexported symbol '%s' at line %d\n", symbol.Id, node.Position.StartLine)
}

func indexOutOfRange(node *parser.ParseTree) {
	util.Printf("Index out of range at line %d\n", node.Position.StartLine)
}
