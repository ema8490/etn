package op

import (
	"ethos/parser"
	"ethos/elTypes"
)

var not_TARGETS = []int{
	t.BOOL,
}

func not(target int, ops ...*parser.ParseTree) (interface{}, int) {
	ok := Expect(1, ops...)
	if !ok {
		return nil, t.UNDEFINED
	}
	child := ops[0]

	switch child.ActualType {
	case t.BOOL:
		v, _ := child.ActualValue.(bool)
		return !v, t.BOOL
	}

	UnexpectedType(child)
	return nil, t.UNDEFINED
}
