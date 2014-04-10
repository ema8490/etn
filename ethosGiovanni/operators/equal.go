package op

import (
	"ethos/elTypes"
	"ethos/parser"
)

var equal_TARGETS = []int{
	t.INT,
	t.FLOAT,
	t.STRING,
	t.BOOL,
	t.MAP,
	t.SET,
	t.TUPLE,
}

func equal(target int, ops ...*parser.ParseTree) (interface{}, int) {
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
			return l == r, t.BOOL
		}
		if right.ActualType == t.FLOAT {
			l, _ := left.ActualValue.(int)
			r, _ := right.ActualValue.(float64)
			return float64(l) == r, t.BOOL
		}
		UnexpectedType(right)
		return nil, t.UNDEFINED
	case t.FLOAT:
		if right.ActualType == t.FLOAT {
			l, _ := left.ActualValue.(float64)
			r, _ := right.ActualValue.(float64)
			return l == r, t.BOOL
		}
		if right.ActualType == t.INT {
			l, _ := left.ActualValue.(float64)
			r, _ := right.ActualValue.(int)
			return l == float64(r), t.BOOL
		}
		UnexpectedType(right)
		return nil, t.UNDEFINED
	case t.STRING:
		if right.ActualType == t.STRING {
			l, _ := left.ActualValue.(string)
			r, _ := right.ActualValue.(string)
			return l == r, t.BOOL
		}
		UnexpectedType(right)
		return nil, t.UNDEFINED
	case t.BOOL:
		if right.ActualType == t.BOOL {
			l, _ := left.ActualValue.(bool)
			r, _ := right.ActualValue.(bool)
			return l == r, t.BOOL
		}
		UnexpectedType(right)
		return nil, t.UNDEFINED
	case t.MAP:
		return equalMap(left, right)
	case t.TUPLE:
		return equalTuple(left, right)
	case t.SET:
		return equalSet(left, right)
	}

	UnexpectedType(left)
	return nil, t.UNDEFINED
}

func equalMap(l *parser.ParseTree, r *parser.ParseTree) (interface{}, int) {
	left, _ := l.ActualValue.(*t.MapRep)
	right, _ := r.ActualValue.(*t.MapRep)
	if len(left.Content) != len(right.Content) || left.Type != right.Type {
		return false, t.BOOL
	}
	for k, e := range left.Content {
		if right.Content[k] == nil {
			return false, t.BOOL
		}
		eq, ok := ElEqual(e, right.Content[k])
		if !ok || !eq {
			return false, t.BOOL
		}
	}
	return true, t.BOOL
}

func equalTuple(l *parser.ParseTree, r *parser.ParseTree) (interface{}, int) {
	left, _ := l.ActualValue.(*t.TupleRep)
	right, _ := r.ActualValue.(*t.TupleRep)
	if left.Len != right.Len || left.Type != right.Type {
		return false, t.BOOL
	}
	for k, e := range left.Content {
		if right.Content[k] == nil {
			return false, t.BOOL
		}
		eq, ok := ElEqual(e, right.Content[k])
		if !ok || !eq {
			return false, t.BOOL
		}
	}
	return true, t.BOOL
}

func equalSet(l *parser.ParseTree, r *parser.ParseTree) (interface{}, int) {
	left, _ := l.ActualValue.(*t.SetRep)
	right, _ := r.ActualValue.(*t.SetRep)
	if len(left.Content) != len(right.Content) || left.Type != right.Type {
		return false, t.BOOL
	}
	for k, e := range left.Content {
		if right.Content[k] == nil {
			return false, t.BOOL
		}
		eq, ok := ElEqual(e, right.Content[k])
		if !ok || !eq {
			return false, t.BOOL
		}
	}
	return true, t.BOOL
}
