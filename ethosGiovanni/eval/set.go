package ev

import (
	"crypto/md5"
	"ethos/elTypes"
	"ethos/operators"
	"ethos/parser"
	"ethos/util"
	"fmt"
	"io"
)

func newSet(elements ...*parser.ParseTree) (interface{}, int) {
	set := new(t.SetRep)
	set.Content = map[string]interface{}{}

	if len(elements) > 0 {
		for i, e := range elements[:len(elements)-1] {
			if e.ActualType == t.UNDEFINED {
				undefined(e)
				return nil, t.UNDEFINED
			}
			if op.CompositeType(e) != op.CompositeType(elements[i+1]) {
				op.UnexpectedType(elements[i+1])
				return nil, t.UNDEFINED
			}

			addElement(set, e)
		}
		addElement(set, elements[len(elements)-1])
		set.Type = elements[0].ActualType
	} else {
		set.Type = t.UNDEFINED
	}

	return set, t.SET
}

func addElement(set *t.SetRep, e interface{}) {
	h := hash(e)
	if set.Content[h] != nil {
		ok, equal := op.ElEqual(set.Content[h], e)
		if ok && !equal {
			//TODO
			util.Printf("Collision in set!\n")
			return
		}
	}
	set.Content[h] = e
}

func hash(e interface{}) string {
	h := md5.New()
	io.WriteString(h, fmt.Sprintf("%v", e))
	return fmt.Sprintf("%x", h.Sum())
}
