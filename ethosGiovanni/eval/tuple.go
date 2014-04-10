package ev

import (
	"ethos/elTypes"
	"ethos/operators"
	"ethos/parser"
)

func newTuple(elements ...*parser.ParseTree) (interface{}, int) {
	tuple := new(t.TupleRep)
	tuple.Content = []interface{}{}

	if len(elements) > 0 {
		for i, e := range elements[:len(elements)-1] {
			if op.CompositeType(e) != op.CompositeType(elements[i+1]) || e.ActualType == t.UNDEFINED {
				op.UnexpectedType(elements[i+1])
				return nil, t.UNDEFINED
			}
			tuple.Content = append(tuple.Content, e) //.ActualValue)
		}

		tuple.Content = append(tuple.Content, elements[len(elements)-1]) //.ActualValue)
		tuple.Type = elements[0].ActualType
	} else {
		tuple.Type = t.UNDEFINED
	}
	tuple.Len = len(elements)

	return tuple, t.TUPLE
}

func accessTuple(node *parser.ParseTree, index int) (interface{}, int) {
	if node.ActualType != t.TUPLE {
		op.UnexpectedType(node)
		return nil, t.UNDEFINED
	}

	tuple, _ := node.ActualValue.(*t.TupleRep)

	if index < 0 || index >= tuple.Len {
		indexOutOfRange(node)
		return nil, t.UNDEFINED
	}

	val, _ := tuple.Content[index].(*parser.ParseTree)
	return val.ActualValue, tuple.Type
}

func tupleLength(node *parser.ParseTree) int {
	if node.ActualType != t.TUPLE {
		op.UnexpectedType(node)
		return 0
	}

	tuple, _ := node.ActualValue.(*t.TupleRep)
	return tuple.Len
}

func setTuple(node, value, index *parser.ParseTree) *t.TupleRep {
	tuple, ok := node.ActualValue.(*t.TupleRep)
	if !ok {
		op.UnexpectedType(node)
		return tuple
	}

	// typecheck
	if tuple.Len > 0 {
		tNode, _ := tuple.Content[0].(*parser.ParseTree)
		if op.CompositeType(tNode) != op.CompositeType(value) {
			op.UnexpectedType(value)
			return tuple
		}
	} else {
		tuple.Type = value.ActualType
	}
	ind, _ := index.ActualValue.(int)
	if ind < 0 || ind >= tuple.Len {
		indexOutOfRange(index)
		return tuple
	}

	tuple.Content[ind] = value
	return tuple
}
