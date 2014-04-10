package ev

import (
	"ethos/elTypes"
	"ethos/operators"
	"ethos/parser"
)

func accessString(node *parser.ParseTree, index int) (interface{}, int) {
	if node.ActualType != t.STRING {
		op.UnexpectedType(node)
		return nil, t.UNDEFINED
	}

	str, _ := node.ActualValue.(string)

	if index < 0 || index >= len(str) {
		indexOutOfRange(node)
		return nil, t.UNDEFINED
	}

	return str[index : index+1], t.STRING
}

func stringLength(node *parser.ParseTree) int {
	if node.ActualType != t.STRING {
		op.UnexpectedType(node)
		return 0
	}

	s, _ := node.ActualValue.(string)
	return len(s)
}

func setString(node, value, index *parser.ParseTree) string {
	dest, _ := node.ActualValue.(string)
	if value.ActualType != t.STRING {
		op.UnexpectedType(value)
		return dest
	}

	ind, _ := index.ActualValue.(int)
	source, _ := value.ActualValue.(string)
	if ind < 0 || ind >= len(dest) {
		indexOutOfRange(index)
		return dest
	}

	dest = dest[0:ind] + string(source[0]) + dest[ind+1:len(dest)]
	return dest
}
