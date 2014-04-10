package op

import (
	"ethos/elTypes"
	"ethos/parser"
	"ethos/util"
	"fmt"
	"strconv"
	"strings"
)

const (
	SUM = iota
	SUB
	MUL
	DIV
	EQUAL
	NOT_EQUAL
	LOWER
	GREATER
	LOWER_EQUAL
	GREATER_EQUAL
	AND
	OR
	NOT
	UNARY_MINUS
)

type Operator struct {
	Targets []int
	Apply   func(target int, params ...*parser.ParseTree) (interface{}, int)
}

var OPERATORS map[int]*Operator

func Init() {
	OPERATORS = map[int]*Operator{
		SUM:           &Operator{sum_TARGETS, sum},
		SUB:           &Operator{sub_TARGETS, sub},
		MUL:           &Operator{mul_TARGETS, mul},
		DIV:           &Operator{div_TARGETS, div},
		EQUAL:         &Operator{equal_TARGETS, equal},
		NOT_EQUAL:     &Operator{notEqual_TARGETS, notEqual},
		LOWER:         &Operator{lower_TARGETS, lower},
		GREATER:       &Operator{greater_TARGETS, greater},
		LOWER_EQUAL:   &Operator{lowerEqual_TARGETS, lowerEqual},
		GREATER_EQUAL: &Operator{greaterEqual_TARGETS, greaterEqual},
		AND:           &Operator{and_TARGETS, and},
		OR:            &Operator{or_TARGETS, or},
		NOT:           &Operator{not_TARGETS, not},
		UNARY_MINUS:   &Operator{unaryMinus_TARGETS, unaryMinus},
	}
}

func Apply(op int, operands ...*parser.ParseTree) (interface{}, int) {
	opTypes := []int{}
	for i := 0; i < len(operands); i++ {
		opTypes = append(opTypes, operands[i].ActualType)
	}
	operator := OPERATORS[op]
	target := t.TypeCheck(opTypes, operator.Targets)
	// util.Printf("base: %v\n", t.ETN_BASES[target])
	if target == t.UNDEFINED {
		util.Printf("Undefined operation for operands: ")
		for i := 0; i < len(operands); i++ {
			util.Printf("%s", t.TYPES[opTypes[i]])
			if i < len(operands)-1 {
				util.Printf(", ")
			} else {
				util.Printf("\n")
			}
		}
		return nil, t.UNDEFINED
	}

	return operator.Apply(target, operands...)
}

func UnexpectedType(el interface{}) {
	node, _ := el.(*parser.ParseTree)
	util.Printf("Unexpected type %s (%T) at line %d\n", CompositeTypeName(CompositeType(node)), node.ActualValue, node.Position.StartLine)
}

func Expect(n int, ops ...*parser.ParseTree) bool {
	if len(ops) != n {
		util.Printf("Wrong operands count (expected: %d, given: %d)\n", n, len(ops))
		return false
	}
	return true
}

func ElEqual(l interface{}, r interface{}) (bool, bool) {
	o1, ok1 := l.(*parser.ParseTree)
	o2, ok2 := r.(*parser.ParseTree)
	if !ok1 || !ok2 {
		return false, false
	}
	eq, _ := Apply(EQUAL, o1, o2)
	eqq, ok := eq.(bool)
	return eqq, ok
}

// Returns a string identifier for the (composite) type
// of the given node
// Assumes a valid (type-consistent) value in input
func CompositeType(node *parser.ParseTree) string {
	c := ""
	for {
		if t.IsComposite(node.ActualType) {
			switch node.ActualType {
			case t.MAP:
				m, _ := node.ActualValue.(*t.MapRep)
				c = c + fmt.Sprintf("%d.", t.MAP)
				node = new(parser.ParseTree)
				if len(m.Content) > 0 {
					for k, _ := range m.Content {
						node, _ = m.Content[k].(*parser.ParseTree)
						break
					}
					continue
				} else {
					node.ActualType = t.UNDEFINED
					// do not continue
				}
				break
			case t.SET:
				s, _ := node.ActualValue.(*t.SetRep)
				c = c + fmt.Sprintf("%d.", t.SET)
				node = new(parser.ParseTree)
				if len(s.Content) > 0 {
					for k, _ := range s.Content {
						node, _ = s.Content[k].(*parser.ParseTree)
						break
					}
					continue
				} else {
					node.ActualType = t.UNDEFINED
					// do not continue
				}
				break
			case t.TUPLE:
				tu, _ := node.ActualValue.(*t.TupleRep)
				c = c + fmt.Sprintf("%d.", t.TUPLE)
				node = new(parser.ParseTree)
				if len(tu.Content) > 0 {
					for k, _ := range tu.Content {
						node, _ = tu.Content[k].(*parser.ParseTree)
						break
					}
					continue
				} else {
					node.ActualType = t.UNDEFINED
					// do not continue
				}
				break
			}

		}
		// not composite
		c = c + fmt.Sprintf("%d.", node.ActualType)
		break
	}
	return c[:len(c)-1]
}

func CompositeTypeName(ct string) string {
	tn := ""
	for _, ts := range strings.Split(ct, ".") {
		ti, _ := strconv.Atoi(ts)
		tn = tn + t.TYPES[ti] + "."
	}
	return tn[:len(tn)-1]
}
