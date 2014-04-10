/*
	This package provides the functionalities for creating the intermediate representation.
*/
package ir

import (
	"ethos/elTypes"
	"ethos/environment"
	"ethos/nodetypes"
	"ethos/operators"
	"ethos/parser"
	"ethos/util"
	"fmt"
)

const (
	UNARY_MINUS = iota
	ADD
	SUB
	MUL
	DIV
	NOT
	OR_EXPRESSION
	AND_EXPRESSION
	L_COMPARISON
	L_E_COMPARISON
	G_COMPARISON
	G_E_COMPARISON
	E_COMPARISON
	N_E_COMPARISON
	VALUE_ACCESS
	SQUARE_ACCESS
	ASSIGNMENT
	FUNCTION_CALL
	JMP
	BEQ
	BNE
)

var OPERATIONS = map[int]string{
	UNARY_MINUS:		 "UNARY_MINUS",
	ADD:                 "ADD",
	SUB:                 "SUB",
	MUL:                 "MUL",
	DIV:                 "DIV",
	NOT:                 "NOT",
	OR_EXPRESSION:       "OR_EXPRESSION",
	AND_EXPRESSION:      "AND_EXPRESSION",
	L_COMPARISON:        "L_COMPARISON",
	L_E_COMPARISON:      "L_E_COMPARISON",
	G_COMPARISON:        "G_COMPARISON",
	G_E_COMPARISON:      "G_E_COMPARISON",
	E_COMPARISON:        "E_COMPARISON",
	N_E_COMPARISON:      "N_E_COMPARISON",
	VALUE_ACCESS:        "VALUE_ACCESS",
	SQUARE_ACCESS:       "SQUARE_ACCESS",
	ASSIGNMENT			 "ASSIGNMENT",	
	FUNCTION_CALL		 "FUNCTION_CALL",
	JMP					 "JMP",
	BEQ					 "BEQ",
	BNE					 "BNE",
}

var HeadInstruction *Instruction = nil

type Instruction struct {
	
	op int
	arg1 *interface{}
	arg2 *interface{}
	res *interface{}
	trueL *Instruction
	falseL *Instruction
	
}

func InsertInTail(instr *Instruction){
	var i *Instruction
	for i := HeadInstruction; i != nil; i = i.trueL{		
	}
	i.trueL = instr
}

func InsertBranch(trueInstr *Instruction, falseInstr *Instruction){
	var i *Instruction
	for i := HeadInstruction; i != nil; i = i.trueL{		
	}
	i.trueL = trueInstr
	i.falseL = falseipIInstr
}

func NewInstruction(op int, arg1, arg2, res, *Symbol, trueL, falseL *Instruction)(instr Instruction){
	instr.op = op
	instr.arg1 = arg1
	instr.arg2 = arg2
	instr.res = res
	instr.trueL = trueL
	instr.falseL = falseL
	return
}

func PrintInstruction(instr Instruction){
	if(instr.arg1!=nil){
		id1:=instr.arg1.Id;
	}else{
		id1:=nil
	}
	if(instr.arg2!=nil){
		id2:=instr.arg2.Id;
	}else{
		id2:=nil
	}
	if(instr.res!=nil){
		id3:=instr.res.Id;
	}else{
		id3:=nil
	}
	fmt.Println(string(&instr)+": "+OPERATIONS[instr.op]+" "+id1+" "+id2+" "+id3+" "+string(instr.trueL)+" "+string(instr.falseL))
}



