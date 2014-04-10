package ast

import (
    "../nodetypes"
)

type ParseTree struct {
	
	NodeKind	int			//the kind of the node
	NodeType   	int			//the type of the node
	Children   	[]*ParseTree//all the nodes depending on this one
	SymbolPtr  	*Symbol 	//pointer to the symbol table entry
	Value		[]byte		//for constant values
	LineNumber 	int			//line number of the declaration of this symbol
	
}

//creates a new node
func NewNode(nodeKind int) *ParseTree {
    node := new(ParseTree)
    node.NodeKind = nodeKind
    return node
}
