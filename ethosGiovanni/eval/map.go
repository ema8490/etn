package ev

import (
	"ethos/elTypes"
	"ethos/io"
	"ethos/operators"
	"ethos/parser"
	"ethos/typeDefs"
	"ethos/util"
)

func newMap(elements ...*parser.ParseTree) (interface{}, int) {
	m := new(t.MapRep)
	m.Content = map[string]interface{}{}

	if len(elements) > 0 {
		for i, e := range elements[:len(elements)-1] {
			elName := e.Children[0]
			elValue := e.Children[1]
			nextElValue := elements[i+1].Children[1]

			if op.CompositeType(elValue) != op.CompositeType(nextElValue) {
				op.UnexpectedType(nextElValue)
				return nil, t.UNDEFINED
			}
			if elValue.ActualType == t.UNDEFINED {
				op.UnexpectedType(elValue)
				return nil, t.UNDEFINED
			}

			elNameString := string(elName.Value)
			m.Content[elNameString] = elValue
		}

		lastElName := elements[len(elements)-1].Children[0]
		lastElValue := elements[len(elements)-1].Children[1]
		lastElNameString := string(lastElName.Value)
		m.Content[lastElNameString] = lastElValue

		m.Type = lastElValue.ActualType
	} else {
		m.Type = t.UNDEFINED
	}

	return m, t.MAP
}

func accessMap(node, index *parser.ParseTree) (interface{}, int) {
	if node.ActualType != t.MAP {
		op.UnexpectedType(node)
		return nil, t.UNDEFINED
	}
	if index.ActualType != t.STRING {
		op.UnexpectedType(index)
		return nil, t.UNDEFINED
	}

	m, _ := node.ActualValue.(*t.MapRep)

	n, _ := index.ActualValue.(string)

	v := m.Content[n]

	if v == nil {
		undefined(index)
		return nil, t.UNDEFINED
	}
	val, _ := v.(*parser.ParseTree)
	return val.ActualValue, m.Type
}

func newMapFromDir(path string) (interface{}, int) {
	names, files := io.ReadTypedDir(path)

	m := new(t.MapRep)
	m.Content = map[string]interface{}{}
	m.Source = path

	// no need to check coherency of files type since
	// it is already enforced
	for i := 0; i < len(files); i++ {
		switch v := files[i].(type) {
		case *typeDefs.ElInt:
			node := new(parser.ParseTree)
			node.ActualValue = int(*v)
			node.ActualType = t.INT
			m.Content[names[i]] = node
			m.Type = t.INT
			break
		case *typeDefs.ElFloat:
			node := new(parser.ParseTree)
			node.ActualValue = float64(*v)
			node.ActualType = t.FLOAT
			m.Content[names[i]] = node
			m.Type = t.FLOAT
			break
		case *typeDefs.ElString:
			node := new(parser.ParseTree)
			node.ActualValue = string(*v)
			node.ActualType = t.STRING
			m.Content[names[i]] = node
			m.Type = t.STRING
			break
		case *typeDefs.ElBool:
			node := new(parser.ParseTree)
			node.ActualValue = bool(*v)
			node.ActualType = t.BOOL
			m.Content[names[i]] = node
			m.Type = t.BOOL
			break
		case *typeDefs.ElTuple:
			node := new(parser.ParseTree)
			tuple := new(t.TupleRep)
			tuple.Len = len(v.Content)
			tuple.Type = int(v.Type)
			tuple.Content = []interface{}{}
			for _, e := range v.Content {
				tuple.Content = append(tuple.Content, e)
			}
			node.ActualType = t.TUPLE
			node.ActualValue = tuple
			m.Content[names[i]] = node
			m.Type = t.TUPLE
			break
		}
	}

	return m, t.MAP
}

func dirAssignment(path string, value interface{}) {
	m, ok := value.(*t.MapRep)
	if !ok {
		util.Printf("Invalid directory assignment: non-MAP as source\n")
		return
	}
	if io.WriteTypedDir(path, m) {
		util.Printf("Invalid directory assignment: invalid type for destination directory\n")
	}
}

func mapNextIndex(node *parser.ParseTree, previous int) string {
	m, _ := node.ActualValue.(*t.MapRep)
	i := 0
	for k, _ := range m.Content {
		i++
		if i > previous {
			return k
		}
	}
	return ""
}

func setMap(node, value, index *parser.ParseTree) *t.MapRep {
	m, ok1 := node.ActualValue.(*t.MapRep)
	if !ok1 {
		op.UnexpectedType(node)
		return m
	}
	// typecheck
	if len(m.Content) > 0 {
		var mNode *parser.ParseTree
		for _, v := range m.Content {
			mNode, _ = v.(*parser.ParseTree)
			if op.CompositeType(mNode) != op.CompositeType(value) {
				op.UnexpectedType(value)
				return m
			}
		}
	} else {
		m.Type = value.ActualType
	}

	ind, ok2 := index.ActualValue.(string)
	if !ok2 {
		op.UnexpectedType(index)
		return m
	}

	m.Content[ind] = value
	return m
}
