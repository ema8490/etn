%{

package parser

import (
    "./nodetypes"
    "fmt"
    "./ast"
)

const (
    EOF = iota

    SKIP
    DIGIT
    ALPHA
    COMMENT_START
    ANY
)

%}

%union{
    value []byte
    node  *ParseTree
}

%token L_PAR
%token R_PAR
%token L_CUR
%token R_CUR
%token L_SQ
%token R_SQ
%token DOT
%token SEMI
%token COLON
%token COMMA
%token DOUBLE_QUOTE
%token EQ
%token EQ_COMP
%token NOT_EQ_COMP
%token L_COMP
%token L_EQ_COMP
%token G_COMP
%token G_EQ_COMP
%token PLUS
%token MINUS
%token MUL
%token SLASH
%token PATH
%token INTEGER_LITERAL
%token FLOAT_LITERAL
%token STRING_LITERAL
%token QUOTED_STRING_LITERAL
%token BOOL_LITERAL
%token DOLLAR
%token IDENTIFIER
%token NOT
%token AND
%token OR
%token AMPERSAND
%token PIPE
%token TOKEN

%token FOR
%token IN
%token IF
%token ELSE
%token BREAK
%token CONTINUE
%token RETURN
%token SWITCH
%token CASE
%token FUNC
%token EXPORT
%token VAR
%token NEW

%nonassoc IF
%nonassoc ELSE
%right EQ
%left EQ_COMP NOT_EQ_COMP
%nonassoc L_COMP G_COMP L_EQ_COMP G_EQ_COMP
%left PLUS MINUS
%left MUL SLASH 
%nonassoc error

%type <value> INTEGER_LITERAL
%type <value> FLOAT_LITERAL
%type <value> STRING_LITERAL
%type <value> QUOTED_STRING_LITERAL
%type <value> BOOL_LITERAL
%type <value> IDENTIFIER
%type <value> PATH
%type <value> TOKEN
%type <value> MINUS

%type <node> identifier
%type <node> int_literal
%type <node> float_literal
%type <node> string_literal
%type <node> bool_literal
%type <node> tuple_literal
%type <node> map_literal
%type <node> path_literal
%type <node> set_literal
%type <node> literal
%type <node> assignment
%type <node> expression
%type <node> bool_expression
%type <node> or_expression
%type <node> and_expression
%type <node> comparison
%type <node> e_comparison
%type <node> n_e_comparison
%type <node> l_comparison
%type <node> l_e_comparison
%type <node> g_comparison
%type <node> g_e_comparison
%type <node> sum
%type <node> add
%type <node> sub
%type <node> product
%type <node> mul
%type <node> div
%type <node> value_access
%type <node> square_access
%type <node> value
%type <node> function_call
%type <node> statement
%type <node> function_definition
%type <node> control_statement
%type <node> return
%type <node> loop
%type <node> if
%type <node> switch
%type <node> foreach
%type <node> identifiers_couple
%type <node> for
%type <node> for_init
%type <node> for_condition
%type <node> for_step
%type <node> assignment_list
%type <node> assignment_list_1
%type <node> if_then_else
%type <node> if_then
%type <node> block
%type <node> statement_list
%type <node> statement_list_1
%type <node> switch_block
%type <node> case_list
%type <node> case_list_1
%type <node> case_else
%type <node> case
%type <node> params_list
%type <node> params_list_1
%type <node> named_params_list
%type <node> named_params_list_1
%type <node> colon_params_list
%type <node> colon_params_list_1
%type <node> colon_param
%type <node> program
%type <node> export
%type <node> var
%type <node> not
//%type <node> pipeline
//%type <node> pipelines
//%type <node> pipeline_1
//%type <node> pipeline_statement
//%type <node> arguments_list_1
//%type <node> argument
//%type <node> command
//%type <node> constructor
//%type <node> redirections
//%type <node> redirection

%start program

%%

identifier: IDENTIFIER {    //DONE
    $$ = newNode(nt.IDENTIFIER)
    $$.Value = $1
    //$$.LineNumber = yyline
};

int_literal : INTEGER_LITERAL {
    $$ = newNode(nt.INT_LITERAL)
    $$.Value = $1
};

float_literal : FLOAT_LITERAL {
    $$ = newNode(nt.FLOAT_LITERAL)
    $$.Value = $1
};

string_literal : STRING_LITERAL {
    $$ = newNode(nt.STRING_LITERAL)
    $$.Value = $1
}
               | QUOTED_STRING_LITERAL {
    $$ = newNode(nt.STRING_LITERAL)
    $$.Value = $1
};

bool_literal : BOOL_LITERAL {
    $$ = newNode(nt.BOOL_LITERAL)
    $$.Value = $1
};

tuple_literal : L_SQ params_list R_SQ {
    $$ = newNode(nt.TUPLE_LITERAL)
    $$.Children = []*ParseTree{}
    if $2 != nil && $2.Children != nil {
        for _, param := range $2.Children {
            $$.Children = append($$.Children, param)
        }
    }
};

map_literal : L_PAR colon_params_list R_PAR {
    $$ = newNode(nt.MAP_LITERAL)
    $$.Children = []*ParseTree{}
    if $2 != nil && $2.Children != nil {
        for _, param := range $2.Children {
            $$.Children = append($$.Children, param)
        }
    }
};

path_literal : PATH {
    $$ = newNode(nt.PATH_LITERAL)
    $$.Value = $1
    $$.Children = []*ParseTree{}
};

set_literal : L_CUR params_list R_CUR {
    $$ = newNode(nt.SET_LITERAL)
    $$.Children = []*ParseTree{}
    if $2 != nil && $2.Children != nil {
        for _, param := range $2.Children {
            $$.Children = append($$.Children, param)
        }
    }
};

literal : int_literal {
    $$ = $1
}
        | float_literal {
    $$ = $1
}
        | string_literal {
    $$ = $1
}
        | bool_literal {
    $$ = $1
}
        | tuple_literal {
    $$ = $1
}
        | map_literal {
    $$ = $1
}
        | path_literal {
    $$ = $1
}
        | set_literal {
    $$ = $1
};

assignment : identifier EQ expression {
    $$ = newNode(nt.ASSIGNMENT)
    $$.Children = []*ParseTree{$1, $3}
}
           | path_literal EQ expression {
    $$ = newNode(nt.ASSIGNMENT)
    $$.Children = []*ParseTree{$1, $3}
}
           | square_access EQ expression {
    $$ = newNode(nt.ASSIGNMENT)
    $$.Children = []*ParseTree{$1, $3}
};

expression : bool_expression {  //DONE
    $$ = $1
};

bool_expression : or_expression {   //DONE
    $$ = $1
};

or_expression : and_expression OR or_expression {   //DONE
    $$ = newNode(nt.OR_EXPRESSION)
    $$.Children = []*ParseTree{$1, $3}
}
              | and_expression {
    $$ = $1
};

and_expression : comparison AND and_expression {    //DONE
    $$ = newNode(nt.AND_EXPRESSION)
    $$.Children = []*ParseTree{$1, $3}
}
              | comparison {
    $$ = $1
};

comparison : e_comparison { //DONE
    $$ = $1
}
           | n_e_comparison {
    $$ = $1
}
           | l_comparison {
    $$ = $1
}
           | l_e_comparison {
    $$ = $1
}
           | g_comparison {
    $$ = $1
}
           | g_e_comparison {
    $$ = $1
}
           | not {
    $$ = $1
}
           | sum {
    $$ = $1
};

l_comparison : sum L_COMP sum { //DONE 
    $$ = newNode(nt.L_COMPARISON)
    $$.Children = []*ParseTree{$1, $3}
};

l_e_comparison : sum L_EQ_COMP sum {    //DONE
    $$ = newNode(nt.L_E_COMPARISON)
    $$.Children = []*ParseTree{$1, $3}
};

g_comparison : sum G_COMP sum { //DONE
    $$ = newNode(nt.G_COMPARISON)
    $$.Children = []*ParseTree{$1, $3}
};

g_e_comparison : sum G_EQ_COMP sum {    //DONE
    $$ = newNode(nt.G_E_COMPARISON)
    $$.Children = []*ParseTree{$1, $3}
};

e_comparison : sum EQ_COMP sum {  //DONE
    $$ = newNode(nt.E_COMPARISON)
    $$.Children = []*ParseTree{$1, $3}
};

n_e_comparison : sum NOT_EQ_COMP sum {   //DONE
    $$ = newNode(nt.N_E_COMPARISON)
    $$.Children = []*ParseTree{$1, $3}
};

not : NOT value {   //DONE
    $$ = newNode(nt.NOT)
    $$.Children = []*ParseTree{$2}
};

sum : add { //DONE
    $$ = $1
}
    | sub {
    $$ = $1
}
    | product {
    $$ = $1
};

add : product PLUS sum {    //DONE
    $$ = newNode(nt.ADD)
    $$.Children = []*ParseTree{$1, $3}
};

sub : product MINUS sum {   //DONE
    $$ = newNode(nt.SUB)
    $$.Children = []*ParseTree{$1, $3}
};

product : mul { //DONE
    $$ = $1
}
        | div {
    $$ = $1
}
        | value {
    $$ = $1
};

mul : product MUL value {   //DONE
    $$ = newNode(nt.MUL)
    $$.Children = []*ParseTree{$1, $3}
};

div : product SLASH value { //DONE
    $$ = newNode(nt.DIV)
    $$.Children = []*ParseTree{$1, $3}
};

value_access : value DOT function_call {    //DONE
    $$ = newNode(nt.VALUE_ACCESS)
    $$.Children = []*ParseTree{$1, $3}
}
             | value DOT STRING_LITERAL {
    $$ = newNode(nt.VALUE_ACCESS)
    id := newNode(nt.IDENTIFIER)
    id.Value = $3
    $$.Children = []*ParseTree{$1, id}
};

square_access : value L_SQ sum R_SQ {   //DONE
    $$ = newNode(nt.SQUARE_ACCESS)
    $$.Children = []*ParseTree{$1, $3}
};

export : EXPORT function_definition {   //DONE
    $$ = newNode(nt.EXPORT)
    $2.SymbolPtr.Exported = true
    $$.Children = []*ParseTree{$2}
}
       | EXPORT var {
    $$ = newNode(nt.EXPORT)
    $2.SymbolPtr.Exported = true
    $$.Children = []*ParseTree{$2}
};

value : value_access {
    $$ = $1
}
      | square_access {
    $$ = $1
}
      | function_call {
    $$ = $1
}
      | literal {
    $$ = $1
}
      | function_definition {
    $$ = $1
}
      | identifier {
        if(TopStackSymbolTable.LookUp(string($1.Value)) != nil){
            $$ = $1
        }else{
            error("Variable %s not declared\n", string($1.Value))
        }
}
      | L_PAR expression R_PAR {
    $$ = $2
}
      | MINUS value {
    $$ = newNode(nt.UNARY_MINUS)
    $$.Children = []*ParseTree{$2}
}/*
      | constructor {
    $$ = $1
}*/;

statement : var {   //DONE
    $$ = $1
}
          | function_definition {
    $$ = $1
}
          | value_access {
    $$ = $1
}
          | control_statement {
    $$ = $1
}
          | function_call {
    $$ = $1
}
          | assignment {
    $$ = $1
}
          | export {
    $$ = $1
}/*
          | pipelines {
    $$ = $1
}*/;

var : VAR identifier EQ expression {    //DONE
    if(TopStackSymbolTable.LookUp(string($2.Value)) != nil){
        $$ = newNode(nt.VAR)
        $2.SymbolPtr = TopStackSymbolTable.Insert(string($2.Value), $4.Value, t.UNDEFINED, false)
        $$.Children = []*ParseTree{$2, $4}
    }else{
        error("Variable %s already declared\n", string($2.Value))
    }
}
    | VAR identifier {
    if(TopStackSymbolTable.LookUp(string($2.Value) != nil)){
        $$ = newNode(nt.VAR)
        $2.SymbolPtr = TopStackSymbolTable.Insert(string($2.Value), nil, t.UNDEFINED, false)
        $$.Children = []*ParseTree{$2}
    }else{
        error("Variable %s already declared\n", string($2.Value))
    }
};

control_statement : BREAK {
    $$ = newNode(nt.BREAK)
}
                  | CONTINUE {
    $$ = newNode(nt.CONTINUE)
}
                  | return {
    $$ = $1
}
                  | loop {
    $$ = $1
}
                  | if {
    $$ = $1
}
                  | switch {
    $$ = $1
};

loop : foreach {
    $$ = $1
}
     | for {
    $$ = $1
};

foreach : FOR identifiers_couple IN value block {
    $$ = newNode(nt.FOREACH)
    $$.Children = []*ParseTree{$2, $4, $5}
};

identifiers_couple : identifier {
    $$ = newNode(nt.IDENTIFIERS_COUPLE)
    $$.Children = []*ParseTree{$1}
}
                   | identifier COMMA identifier {
    $$ = newNode(nt.IDENTIFIERS_COUPLE)
    $$.Children = []*ParseTree{$1, $3}
};

for : FOR for_init SEMI for_condition SEMI for_step block {
    $$ = newNode(nt.FOR)
    $$.Children = []*ParseTree{$2, $4, $6, $7}
};

for_init : assignment_list {
    $$ = newNode(nt.FOR_INIT)
    if $1 != nil && $1.Children != nil {
        $$.Children = $1.Children
    } else {
        $$.Children = []*ParseTree{}
    }
};

for_condition : expression {
    $$ = newNode(nt.FOR_CONDITION)
    $$.Children = []*ParseTree{$1}
};

for_step : assignment_list {
    $$ = newNode(nt.FOR_STEP)
    if $1 != nil && $1.Children != nil {
        $$.Children = $1.Children
    } else {
        $$.Children = []*ParseTree{}
    }
};

assignment_list : /* empty */ {
    $$ = nil
}
                | assignment_list_1 {
    $$ = $1
};

assignment_list_1 : assignment COMMA assignment_list_1 {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
    if $3 != nil && $3.Children != nil {
        for _, assignment := range $3.Children {
            $$.Children = append($$.Children, assignment)
        }
    }
}
                  | assignment {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
};

if : if_then_else {
    $$ = $1
}
   | if_then {
    $$ = $1
};

if_then : IF expression block {
    $$ = newNode(nt.IFTHEN)
    $$.Children = []*ParseTree{$2, $3}
};

if_then_else : IF expression block ELSE block {
    $$ = newNode(nt.IFTHENELSE)
    $$.Children = []*ParseTree{$2, $3, $5}
};

return : RETURN expression {
    $$ = newNode(nt.RETURN)
    $$.Children = []*ParseTree{$2}
}
       | RETURN SEMI {
    $$ = newNode(nt.RETURN)
    $$.Children = []*ParseTree{}
};

switch : SWITCH expression switch_block {
    $$ = newNode(nt.SWITCH)
    $$.Children = []*ParseTree{$2}
    if $3 != nil && $3.Children != nil {
        for _, c := range $3.Children {
            $$.Children = append($$.Children, c)
        }
    }
};

block : L_CUR statement_list R_CUR {
    $$ = newNode(nt.BLOCK)
    if $2 != nil && $2.Children != nil {
        $$.Children = $2.Children
    } else {
        $$.Children = []*ParseTree{}
    }
};

statement_list : /* empty */ {  //DONE
    $$ = nil
}
                | statement_list_1 {
    $$ = $1
};

statement_list_1 : statement statement_list_1 { //DONE
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
    if $2 != nil && $2.Children != nil {
        for _, statement := range $2.Children {
            $$.Children = append($$.Children, statement)
        }
    }
}
                  | statement {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
};

/*pipelines : pipeline SEMI pipelines {
    $$ = newNode(nt.PIPELINES)
    $$.Children = []*ParseTree{$1}
    $$.Children = append([]*ParseTree{$1}, new(ParseTree))
    $$.Children = append($$.Children, $3.Children...)
}
          | pipeline AND pipelines {
    $$ = newNode(nt.PIPELINES)
    $$.Children = append([]*ParseTree{$1}, newNode(nt.AND_EXPRESSION))
    $$.Children = append($$.Children, $3.Children...)
}
          | pipeline OR pipelines {
    $$ = newNode(nt.PIPELINES)
    $$.Children = append([]*ParseTree{$1}, newNode(nt.OR_EXPRESSION))
    $$.Children = append($$.Children, $3.Children...)
}
          | pipeline {
    $$ = newNode(nt.PIPELINES)
    $$.Children = []*ParseTree{$1}
};

pipeline : pipeline_1 AMPERSAND {
    $$ = newNode(nt.PIPELINE_BG)
    $$.Children = $1.Children
}
         | pipeline_1 {
    $$ = newNode(nt.PIPELINE)
    $$.Children = $1.Children
};

pipeline_1 : pipeline_statement PIPE pipeline_1 {
    $$ = new(ParseTree)
    $$.Children = append([]*ParseTree{$1}, $3.Children...)
}
           | pipeline_statement {
    $$ = newNode(nt.PIPELINE)
    $$.Children = []*ParseTree{$1}
};

pipeline_statement : command arguments_list_1 redirections {
    $$ = newNode(nt.EXEC)
    id := newNode(nt.IDENTIFIER)
    id.Value = $1.Value
    $$.Children = append([]*ParseTree{id}, $2.Children...)
    $$.Children = append($$.Children, $3)
}
                   | command redirections {
    $$ = newNode(nt.EXEC)
    id := newNode(nt.IDENTIFIER)
    id.Value = $1.Value
    $$.Children = append([]*ParseTree{id}, $2)
};

command : STRING_LITERAL {
    $$ = new(ParseTree)
    $$.Value = $1
}
        | PATH {
    $$ = new(ParseTree)
    $$.Value = $1
};

redirections : redirection redirection {
    $$ = newNode(nt.REDIRECTIONS)
    $$.Children = []*ParseTree{$1, $2}
}
             | redirection {
    $$ = newNode(nt.REDIRECTIONS)
    $$.Children = []*ParseTree{$1}
}
             | {
    $$ = newNode(nt.REDIRECTIONS)
    $$.Children = []*ParseTree{}
};

redirection : L_COMP PATH {
    $$ = newNode(nt.REDIR_IN)
    $$.Value = $2
}
            | G_COMP PATH {
    $$ = newNode(nt.REDIR_OUT)
    $$.Value = $2
};*/

/*arguments_list_1 : argument arguments_list_1 {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
    if $2 != nil && $2.Children != nil {
        for _, arg := range $2.Children {
            $$.Children = append($$.Children, arg)
        }
    }
}
                 | argument {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
};

argument : identifier {
    $$ = $1
}
         | literal {
    $$ = newNode(nt.STRING_LITERAL)
    $$.Value = $1.Value
}
         | DOT {
    $$ = newNode(nt.STRING_LITERAL)
    $$.Value = []byte{'.'}
}
         | SLASH {
    $$ = newNode(nt.STRING_LITERAL)
    $$.Value = []byte{'/'}
}
         | TOKEN {
    $$ = newNode(nt.STRING_LITERAL)
    $$.Value = $1
}
         | MINUS argument {
    $$ = newNode(nt.STRING_LITERAL)
    $$.Value = append($1, $2.Value...)
};*/

switch_block : L_CUR case_list R_CUR {
    $$ = new(ParseTree)
    // append cases
    if $2 != nil && $2.Children != nil {
        $$.Children = $2.Children
    } else {
        $$.Children = []*ParseTree{}
    }
}
             | L_CUR case_list case_else R_CUR {
    $$ = new(ParseTree)
    // append cases
    if $2 != nil && $2.Children != nil {
        $$.Children = $2.Children
    } else {
        $$.Children = []*ParseTree{}
    }
    // append else
    $$.Children = append($$.Children, $3)
};

case_list : /* empty */ {
    $$ = nil
}
          | case_list_1 {
    $$ = $1
};

case_list_1 : case case_list_1 {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
    if $2 != nil && $2.Children != nil {
        for _, c := range $2.Children {
            $$.Children = append($$.Children, c)
        }
    }
}
            | case {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
};

case : CASE expression COLON block {
    $$ = newNode(nt.CASE)
    $$.Children = []*ParseTree{$2, $4}
};

case_else : ELSE COLON block {
    $$ = newNode(nt.CASE_ELSE)
    $$.Children = []*ParseTree{$3}
};

function_call : expression L_PAR params_list R_PAR {    //DONE
    $$ = newNode(nt.FUNCTION_CALL)
    $$.Children = []*ParseTree{$1}
    if $3 != nil && $3.Children != nil {
        for _, param := range $3.Children {
            $$.Children = append($$.Children, param)
        }
    }
}
              | STRING_LITERAL L_PAR params_list R_PAR {
    if(TopStackSymbolTable.LookUp(string($1)) != nil){
        $$ = newNode(nt.FUNCTION_CALL)
        $$.SymbolPtr = TopStackSymbolTable.LookUp(string($1))
        id := newNode(nt.IDENTIFIER)
        id.Value = $1
        $$.Children = []*ParseTree{id}
        if $3 != nil && $3.Children != nil {
            for _, param := range $3.Children {
                $$.Children = append($$.Children, param)
            }
        }
    }else{
        error("Function %s not declared\n", string($1))
    }
};

params_list : /* empty */ { //DONE
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{}
}
            | params_list_1 {
    $$ = $1
};

params_list_1 : expression COMMA params_list_1 {    //DONE
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
    if $3 != nil && $3.Children != nil {
        for _, expression := range $3.Children {
            $$.Children = append($$.Children, expression)
        }
    }
}
              | expression {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
};

colon_params_list : /* empty */ {   //DONE
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{}
}
            | colon_params_list_1 {
    $$ = $1
};

colon_params_list_1 : colon_param COMMA colon_params_list_1 {   //DONE
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
    if $3 != nil && $3.Children != nil {
        for _, expression := range $3.Children {
            $$.Children = append($$.Children, expression)
        }
    }
}
              | colon_param {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
};

colon_param : STRING_LITERAL COLON expression { //TODO
    $$ = newNode(nt.COLON_PARAM)
    id := newNode(nt.IDENTIFIER)
    id.Value = $1
    $$.Children = []*ParseTree{id, $3}
};

function_definition : FUNC STRING_LITERAL L_PAR named_params_list R_PAR block {
    $$ = newNode(nt.FUNCTION_DEFINITION)
    id := newNode(nt.IDENTIFIER)
    id.Value = $2
    $$.Children = []*ParseTree{id}
    if $4 != nil && $4.Children != nil {
        for _, named_param := range $4.Children {
            $$.Children = append($$.Children, named_param)
        }
    }
    $$.Children = append($$.Children, $6)
}
                    | FUNC L_PAR named_params_list R_PAR block {
    $$ = newNode(nt.FUNCTION_DEFINITION)
    $$.Children = []*ParseTree{nil}
    if $3 != nil && $3.Children != nil {
        for _, named_param := range $3.Children {
            $$.Children = append($$.Children, named_param)
        }
    }
    $$.Children = append($$.Children, $5)
};

named_params_list : /* empty */ {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{}
}
                  | named_params_list_1 {
    $$ = $1
};

named_params_list_1 : identifier COMMA named_params_list_1 {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
    if $3 != nil && $3.Children != nil {
        for _, expression := range $3.Children {
            $$.Children = append($$.Children, expression)
        }
    }
}
                    | identifier {
    $$ = new(ParseTree)
    $$.Children = []*ParseTree{$1}
};

/*constructor : NEW STRING_LITERAL L_CUR colon_params_list R_CUR {
    $$ = newNode(nt.CONSTRUCTOR)
    id := newNode(nt.IDENTIFIER)
    id.Value = $2
    $$.Children = []*ParseTree{id, $4}
}
            | NEW STRING_LITERAL L_CUR params_list R_CUR {
    $$ = newNode(nt.CONSTRUCTOR)
    id := newNode(nt.IDENTIFIER)
    id.Value = $2
    $$.Children = []*ParseTree{id, $4}
};*/

program : {
        Init()
    }
    statement_list {  //DONE
    $$ = newNode(nt.PROGRAM)
    if $2 != nil && $2.Children != nil {
        $$.Children = $2.Children
    } else {
        $$.Children = []*ParseTree{}
    }

    parsed = $$
};

%%

var parsed *ParseTree

/*func newNode(t int) *ParseTree {
    node := new(ParseTree)
    node.Type = t
    return node
}*/

func Error(format string, params ...interface{}) {
    fmt.Printf(format, params...)
    os.Exit(0)
}

func Parse(lexer *Lexer) *ParseTree {
    yyParse(lexer)
    return parsed
}