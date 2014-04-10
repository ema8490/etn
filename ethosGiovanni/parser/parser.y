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
%token NEW
%token NEW_LINE

%nonassoc IF
%nonassoc ELSE
%right EQ
%left EQ_COMP NOT_EQ_COMP
%nonassoc L_COMP G_COMP L_EQ_COMP G_EQ_COMP
%left PLUS MINUS
%left MUL SLASH 
%nonassoc error

//%type <value> INTEGER_LITERAL
//%type <value> FLOAT_LITERAL
//%type <value> STRING_LITERAL
//%type <value> QUOTED_STRING_LITERAL
//%type <value> BOOL_LITERAL
//%type <value> IDENTIFIER
//%type <value> PATH
//%type <value> TOKEN
//%type <value> MINUS

//%type <node> identifier
//%type <node> int_literal
//%type <node> float_literal
//%type <node> string_literal
//%type <node> bool_literal
//%type <node> tuple_literal
//%type <node> map_literal
//%type <node> path_literal
//%type <node> set_literal
//%type <node> literal
//%type <node> assignment
//%type <node> expression
//%type <node> bool_expression
//%type <node> or_expression
//%type <node> and_expression
//%type <node> comparison
//%type <node> e_comparison
//%type <node> n_e_comparison
//%type <node> l_comparison
//%type <node> l_e_comparison
//%type <node> g_comparison
//%type <node> g_e_comparison
//%type <node> sum
//%type <node> add
//%type <node> sub
//%type <node> product
//%type <node> mul
//%type <node> div
//%type <node> value_access
//%type <node> square_access
//%type <node> value
//%type <node> function_call
//%type <node> statement
//%type <node> function_definition
//%type <node> control_statement
//%type <node> return
//%type <node> loop
//%type <node> if
//%type <node> switch
//%type <node> foreach
//%type <node> identifiers_couple
//%type <node> for
//%type <node> for_init
//%type <node> for_condition
//%type <node> for_step
//%type <node> assignments
//%type <node> assignments_list
//%type <node> if_then_else
//%type <node> if_then
//%type <node> block
//%type <node> statements_list
//%type <node> statement_list_1
//%type <node> switch_block
//%type <node> switch_cases
//%type <node> switch_cases_list
//%type <node> switch_case_else
//%type <node> switch_case
//%type <node> params
//%type <node> params_list
//%type <node> named_params_list
//%type <node> named_params_list_1
//%type <node> colon_params
//%type <node> colon_params_list
//%type <node> colon_param
//%type <node> program
//%type <node> export
//%type <node> var
//%type <node> not
//%type <node> pipeline
//%type <node> pipeline_list
//%type <node> pipeline_statements_list
//%type <node> pipeline_statement
//%type <node> arguments_list
//%type <node> argument
//%type <node> command
//%type <node> constructor
//%type <node> redirection_list
//%type <node> redirection

%start program

%%

// The program is a list of exports (only at the beginning) and statements
program : export_list statements_list {  

        }
        | export_list {

        }
        | statements_list {

};

// List of export statements
export_list : export export_list {

            }
            | export {

};

// Export statement for function definition, identifiers
export :  EXPORT function_definition NEW_LINE {   
    
          }
          | EXPORT identifier NEW_LINE {
    
          }
          | EXPORT assignment NEW_LINE {

};

// This rules generates one or more statements. They can be empty (at least a NEW_LINE is required)
statements_list :  statement statements_list {  
    
                }
                | statement {

};

// The main types of statements
statement : expression_statement {

          }
          | control_statement {
    
          }
          | NEW_LINE {  // empty statement

};

// The main kind of expressions
expression_statement  : assignment NEW_LINE{  // Assignment expression

                      }
                      | expression NEW_LINE{  // General expression (RHS of an assignment)

                      }
                      | pipeline_list NEW_LINE {  // Pipeline expressions 

};

// An identifier can only be the result of an expression or a function definition (El variables are both values and functions)
assignment  : identifier EQ expression {
    
            }
            | identifier EQ function_definition{

};

// El identifier for variables (strings starting with '$')
identifier: IDENTIFIER {
    
};

// The more you expand an expression, the higher is the priority (OR < AND < CMP/NOT < PLUS/MINUS < MULT/DIV < value)
expression : bool_expression {

};

bool_expression : or_expression {

};

or_expression : and_expression OR or_expression {

              }
              | and_expression {
    
};

and_expression  :  comparison AND and_expression {

                }
                |   comparison {

};

comparison  : e_comparison {
    
            }
            | n_e_comparison {
    
            }
            | l_comparison {
    
            }
            | l_e_comparison {
    
            }
            | g_comparison {
    
            }
            | g_e_comparison {
    
            }
            | not {
    
            }
            | sum {

};

l_comparison : sum L_COMP sum {  

};

l_e_comparison : sum L_EQ_COMP sum {    

};

g_comparison : sum G_COMP sum { 

};

g_e_comparison : sum G_EQ_COMP sum {    

};

e_comparison : sum EQ_COMP sum {  

};

n_e_comparison : sum NOT_EQ_COMP sum {   

};

not : NOT value {   
    
};

sum : add { 

    }
    | sub {
    
    }
    | product {
    
};

add : product PLUS sum {    
    
};

sub : product MINUS sum {   
    
};

product : mul { 
    
        }
        | div {
    
        }
        | value {
    
};

mul : product MUL value {   
    
};

div : product SLASH value { 
    
};

// All types of possible operands for an expression
value : access_list {

      }
      | literal {
    
      }
      | identifier {
        
      }
      | L_PAR expression R_PAR {
    
      }
      | MINUS value {
    
      }
      | constructor {
   
};

// All kinds of operand accesses. It contains: square accesses, value accesses, function calls and chains of them
access_list : access_value chain_access {  

};

// Operands for which an access (value access, square access, function call) can be made
access_value: identifier { 
    
            }
            |  path_literal { 
    
            }
            |  L_PAR expression R_PAR {

            }
            |  constructor {

};

// All kinds of chains of calls/accesses
chain_access  : DOT STRING_LITERAL chain_access { // Value access and more

              }
              | DOT STRING_LITERAL { // Value access (chain end)

              }
              | L_SQ sum R_SQ chain_access { // Square access and more

              }
              | L_SQ sum R_SQ { // Square access (chain end)

              }
              | function_call_params chain_access { // function call and more

              } 
              | function_call_params { // function call (chain end)

};

// Function call parameters
function_call_params  : L_PAR params R_PAR {

};

// Parameter list can be empty
params  : /* empty */ { 
              
        }
        | params_list {
    
};

// Parameters are expressions
params_list : expression COMMA params_list {    
                
              }
              | expression {
    
};

// All kinds of literals
literal : int_literal {

        }
        | float_literal {

        }
        | string_literal {

        }
        | bool_literal {

        }
        | tuple_literal {

        }
        | map_literal {

        }
        | path_literal {

        }
        | set_literal {

};

int_literal : INTEGER_LITERAL {

};

float_literal : FLOAT_LITERAL {

};

string_literal :  STRING_LITERAL {
    
                }
                | QUOTED_STRING_LITERAL {

};

bool_literal : BOOL_LITERAL {

};

// Example: [$a, 2, HELLO]
tuple_literal : L_SQ params R_SQ {
    
};

path_literal : PATH {
    
};

set_literal : L_CUR params R_CUR {
   
};

// Example: (Name: Jon, Lastname: Solworth)
map_literal : L_PAR colon_params R_PAR {

};

// Colon parameters can be empty
colon_params : /* empty */ {   
                    
                  }
                  | colon_params_list {

};

// List of colon parameters
colon_params_list : colon_param COMMA colon_params_list {   

                    }
                    | colon_param {

};

// A colon parameter is a string literal followed by ':' followed by an expression 
colon_param : STRING_LITERAL COLON expression { //TODO

};

// A function definition must start with the keyword 'func'
function_definition : FUNC L_PAR named_params R_PAR L_CUR statements_list R_CUR {

};

// Named parameters can be empty
named_params :  /* empty */ {
                
              }
              | named_params_list {

};

// Named parameters must be comma-separated identifiers
named_params_list : identifier COMMA named_params_list {

                  }
                  | identifier {

};

// All kinds of control statements
control_statement : return {
    
                  }
                  | loop {
    
                  }
                  | if {
    
                  }
                  | switch {

};

// A loop can be a foreach or a for
loop  : foreach {
    
      }
      | for {

};

// For each element of 'value', where 'value' will have tuple or map type (run-time check)
foreach : FOR identifiers_couple IN value block {

};

// Identifiers in a loop
identifiers_couple :  identifier { // Only value
    
                    }
                    | identifier COMMA identifier { // Key and value

};

// Classical for structure with initialization, condition and step
for : FOR for_init SEMI for_condition SEMI for_step block {

};

// For initialization
for_init : assignments {

};

// For condition
for_condition : expression {

};

// For step
for_step : assignments {

};

// Assignment in for initialization may be empty
assignments : /* empty */ {
    
                }
                | assignments_list {
    
};

// List of comma-separated assignments
assignments_list : assignment COMMA assignments_list {
                    
                  }
                  | assignment {

};

// If control statement
if :  if_then_else {

    }
    | if_then {

    };

// Only if_then
if_then : IF expression block {

};

// If_then_else
if_then_else : IF expression block ELSE block {

};

// Return statement
return  : RETURN expression NEW_LINE{
    
        }
        | RETURN NEW_LINE{
    
};

// Switch statement
switch : SWITCH expression switch_block {

};

// Switch body
switch_block :  L_CUR switch_cases R_CUR {
    
              }
              | L_CUR switch_cases switch_case_else R_CUR {

};

// Switch case can be empty
switch_cases : /* empty */ {
    
          }
          | switch_cases_list {
  
};

// List of switch cases
switch_cases_list : switch_case switch_cases_list {

            }
            | switch_case {

};

// Switch cases definition
switch_case : CASE expression COLON block {

};

// Default switch case
switch_case_else : ELSE COLON block {

};

// The body of a control statement
block : L_CUR block_statement_list R_CUR {
    
      }
      | L_CUR R_CUR { // It can be empty

};

// The list of statement inside a control statement (BREAK and CONTINUE can only appear here)
block_statement_list  : statement block_statement_list{

                      }
                      | BREAK block_statement_list{

                      }
                      | CONTINUE block_statement_list{

                      }
                      | statement{

                      }
                      | BREAK{

                      }
                      | CONTINUE{

};

// Constructor definition??????????
constructor : NEW STRING_LITERAL L_CUR colon_params_list R_CUR {

            }
            | NEW STRING_LITERAL L_CUR params_list R_CUR {

            }
            | NEW STRING_LITERAL L_CUR R_CUR {

};

// List of pipelines
pipeline_list : pipeline SEMI pipeline_list { // More pipelines on the same line
            
          }
          | pipeline AND pipeline_list { // The second pipeline is executed if and only if there is no error in the first one
            
          }
          | pipeline OR pipeline_list { // The second pipeline is executed if and only if there is an error in the first one????
            
          }
          | pipeline {

};

// The AMPERSAND is used to put the pipeline execution in background
pipeline :  pipeline_statements_list AMPERSAND {
    
          }
          | pipeline_statements_list {

};

// List of pipeline statements
pipeline_statements_list :  pipeline_statement PIPE pipeline_statements_list {
    
            }
            | pipeline_statement {

};

// Pipeline command, redirected in case
pipeline_statement  :  command arguments_list redirection_list {
                      
                    }
                    | command redirection_list {
    
                    }
                    | command arguments_list {
    
                    }
                    | command {
    
};

// A command must start with ':'
command : COLON STRING_LITERAL {
    
        }
        | COLON path_literal {

};

// List of command arguments
arguments_list  : arguments_list argument  {

                }
                | argument {

};

// All kinds of command arguments
argument  : identifier {
            
          }
          | literal {
   
          }
          | DOT { // Current directory
           
          }
          | TOKEN {
            
          }
          | MINUS argument {
    
};

// List of redirections
redirection_list  : redirection redirection_list {
    
                  }
                  | redirection {
    
};

// A redirection is a '<' or '>' followed by a PATH
redirection : L_COMP PATH {
    
            }
            | G_COMP PATH {

};

%%

/*
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