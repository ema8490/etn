package op

import (
	"ethos/parser"
	"ethos/elTypes"
)

var unaryMinus_TARGETS = []int{
	t.INT,
	t.FLOAT,
}

func unaryMinus(target int, ops ...*parser.ParseTree) (interface{}, int) {
	ok := Expect(1, ops...)
	if !ok {
		return nil, t.UNDEFINED
	}
	child := ops[0]

	switch child.ActualType {
	case t.INT:
		v, _ := child.ActualValue.(int)
		return -v, t.INT
	case t.FLOAT:
		v, _ := child.ActualValue.(float64)
		return -v, t.FLOAT
	}

	UnexpectedType(child)
	return nil, t.UNDEFINED
}
