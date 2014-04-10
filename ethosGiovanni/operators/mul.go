package op

import (
	"ethos/parser"
	"ethos/elTypes"
)

var mul_TARGETS = []int{
	t.INT,
	t.FLOAT,
}

func mul(target int, ops ...*parser.ParseTree) (interface{}, int) {
	ok := Expect(2, ops...)
	if !ok {
		return nil, t.UNDEFINED
	}
	left, right := ops[0], ops[1]

	switch left.ActualType {
	case t.INT:
		if right.ActualType == t.INT {
			l, _ := left.ActualValue.(int)
			r, _ := right.ActualValue.(int)
			return l * r, t.INT
		}
		if right.ActualType == t.FLOAT {
			l, _ := left.ActualValue.(int)
			r, _ := right.ActualValue.(float64)
			return float64(l) * r, t.FLOAT
		}
		UnexpectedType(right)
		return nil, t.UNDEFINED
	case t.FLOAT:
		if right.ActualType == t.FLOAT {
			l, _ := left.ActualValue.(float64)
			r, _ := right.ActualValue.(float64)
			return l * r, t.FLOAT
		}
		if right.ActualType == t.INT {
			l, _ := left.ActualValue.(float64)
			r, _ := right.ActualValue.(int)
			return l * float64(r), t.FLOAT
		}
		UnexpectedType(right)
		return nil, t.UNDEFINED
	}

	UnexpectedType(left)
	return nil, t.UNDEFINED
}
