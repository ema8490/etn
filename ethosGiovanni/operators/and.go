package op

import (
	"ethos/parser"
	"ethos/elTypes"
)

var and_TARGETS = []int{
	t.BOOL,
}

func and(target int, ops ...*parser.ParseTree) (interface{}, int) {
	ok := Expect(2, ops...)
	if !ok {
		return nil, t.UNDEFINED
	}
	left, right := ops[0], ops[1]

	switch left.ActualType {
	case t.BOOL:
		if right.ActualType == t.BOOL {
			l, _ := left.ActualValue.(bool)
			r, _ := right.ActualValue.(bool)
			return l && r, t.BOOL
		}
		UnexpectedType(right)
		return nil, t.UNDEFINED
	}

	UnexpectedType(left)
	return nil, t.UNDEFINED
}
