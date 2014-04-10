package parser

type ParseTree struct {
	Value       []byte
	Type        int
	Children    []*ParseTree
	Position    InputPosition
	ActualValue interface{}
	ActualType  int
	ActualId    string
}

type InputPosition struct {
	StartPosition int
	EndPosition   int
	StartLine     int
	EndLine       int
}

type Walker func(level int, node *ParseTree, env interface{}) bool

func (self *ParseTree) Walk(level int, walkerDown, walkerUp Walker, env interface{}) {
	if self == nil {
		return
	}
	if walkerDown(level, self, env) {
		return
	}
	for _, child := range self.Children {
		child.Walk(level+1, walkerDown, walkerUp, env)
	}
	walkerUp(level, self, env)
}

func (self *ParseTree) cpy(clean bool) *ParseTree {
	node := new(ParseTree)
	node.Value = self.Value
	node.Type = self.Type
	node.Position = self.Position
	if !clean {
		node.ActualValue = self.ActualValue
		node.ActualType = self.ActualType
		node.ActualId = self.ActualId
	}
	node.Children = []*ParseTree{}
	for _, child := range self.Children {
		node.Children = append(node.Children, child.cpy(clean))
	}
	return node
}

func (self *ParseTree) Copy() *ParseTree {
	return self.cpy(false)
}

func (self *ParseTree) Clean() *ParseTree {
	return self.cpy(true)
}
