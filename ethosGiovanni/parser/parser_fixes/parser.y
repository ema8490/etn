%{

package main

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
//%token VAR
%token NEW
%token UNDERSCORE
%token NEW_LINE
%token TMP

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
//%type <node> assignment_list
//%type <node> assignment_list_1
//%type <node> if_then_else
//%type <node> if_then
//%type <node> block
//%type <node> statement_list
//%type <node> statement_list_1
//%type <node> switch_block
//%type <node> case_list
//%type <node> case_list_1
//%type <node> case_else
//%type <node> case
//%type <node> params_list
//%type <node> params_list_1
//%type <node> named_params_list
//%type <node> named_params_list_1
//%type <node> colon_params_list
//%type <node> colon_params_list_1
//%type <node> colon_param
//%type <node> program
//%type <node> export
//%type <node> var
//%type <node> not
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

program : statement_list {

};

statement_list :  statement statement_list { //DONE
    
                  }
                  | statement {

};

statement : stmt_expression {

            }
            | control_statement {
    
            }
            | export {
    
            }
            | NEW_LINE {

};

stmt_expression : expression NEW_LINE{

                }
                | assignment NEW_LINE{

                }
//              | function_definition SEMI{

//              }
                | pipelines NEW_LINE {

};

expression : bool_expression {  //DONE

};

assignment : identifier EQ expression {    //DONE
    
            }
            | identifier EQ function_definition{

};

identifier: IDENTIFIER {    //DONE
    
};

bool_expression : or_expression {   //DONE

};

or_expression : and_expression OR or_expression {

                }   //DONE
                | and_expression {
    
};

and_expression :  comparison AND and_expression {

                  }   //DONE
                  | comparison {

};

comparison :  e_comparison { //DONE
    
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

l_comparison : sum L_COMP sum { //DONE 

};

l_e_comparison : sum L_EQ_COMP sum {    //DONE

};

g_comparison : sum G_COMP sum { //DONE

};

g_e_comparison : sum G_EQ_COMP sum {    //DONE

};

e_comparison : sum EQ_COMP sum {  //DONE

};

n_e_comparison : sum NOT_EQ_COMP sum {   //DONE

};

not : NOT value {   //DONE
    
};

sum : add { //DONE

      }
      | sub {
    
      }
      | product {
    
};

add : product PLUS sum {    //DONE
    
      };

sub : product MINUS sum {   //DONE
    
      };

product : mul { //DONE
    
          }
          | div {
    
          }
          | value {
    
};

mul : product MUL value {   //DONE
    
};

div : product SLASH value { //DONE
    
};

value_access :  //other_values DOT function_call {    //DONE
    
//              }
              //| 
                other_values DOT STRING_LITERAL {
    
};

square_access : other_values L_SQ sum R_SQ {   //DONE
    
};

other_values: identifier { 
    
            }
            | function_call { 
    
            }
            |  path_literal { 
    
            }
            |  L_PAR expression R_PAR {

            }
            |  constructor {

};

value : value_access {

        }
        | square_access {

        }
        | function_call_list {

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

tuple_literal : L_SQ params_list R_SQ {
    
};

map_literal : L_PAR colon_params_list R_PAR {

};

path_literal : PATH {
    
};

set_literal : L_CUR params_list R_CUR {
   
};

function_call_list: function_call function_call_list {

                    } 
                    | function_call {

};


function_call : //square_access L_PAR  R_PAR {    //DONE
    
//              }
              //| 
                square_access L_PAR params_list R_PAR {
    
                }
                | value_access L_PAR params_list R_PAR {
    
                }

//              |  STRING_LITERAL L_PAR  R_PAR {    //DONE
    
//              }
//              | STRING_LITERAL L_PAR params_list R_PAR {
    
//              }
                | identifier L_PAR params_list R_PAR {
    
};

params_list : /* empty */ { //DONE
              
              }
              | params_list_1 {
    
};

params_list_1 : expression COMMA params_list_1 {    //DONE
                
                }
                | expression {
    
};

function_definition : //FUNC STRING_LITERAL L_PAR named_params R_PAR block {
    
//                    }
//                    | 
                      FUNC L_PAR named_params R_PAR block {

};

named_params :  /* empty */ {
                
                }
                | named_params_list {

};

named_params_list : identifier COMMA named_params_list {

                    }
                    | identifier {

};

block : L_CUR /*statement_list*/ block_stmts R_CUR {
    
        }
        | L_CUR /*statement_list block_stmts*/ R_CUR {

};

block_stmts : statement block_stmts{

              }
              | BREAK block_stmts{

              }
              | CONTINUE block_stmts{

              }
              | statement{

              }
              | BREAK{

              }
              | CONTINUE{

};

export :  EXPORT function_definition SEMI {   //DONE
    
          }
          | EXPORT identifier SEMI {
    
          }
          | EXPORT assignment SEMI {

};

control_statement : //BREAK {
    
                    //}
                    //| CONTINUE {
    
                    //}
                    //| 
                    return {
    
                    }
                    | loop {
    
                    }
                    | if {
    
                    }
                    | switch {

};

loop :  foreach {
    
        }
        | for {

};

foreach : FOR identifiers_couple IN value block {

};

identifiers_couple :  identifier {
    
                      }
                      | identifier COMMA identifier {

};

for : FOR for_init SEMI for_condition SEMI for_step block {

};

for_init : assignment_list {

};

for_condition : expression {

};

for_step : assignment_list {

};

assignment_list : /* empty */ {
    
                  }
                  | assignment_list_1 {
    
};

assignment_list_1 : assignment COMMA assignment_list_1 {
                    
                    }
                    | assignment {

};

if :  if_then_else {

      }
      | if_then {

      };

if_then : IF expression block {

};

if_then_else : IF expression block ELSE block {

};

return :  RETURN expression SEMI{
    
          }
          | RETURN SEMI {
    
};

switch : SWITCH expression switch_block {

};

switch_block :  L_CUR case_list R_CUR {
    
                }
                | L_CUR case_list case_else R_CUR {

};

case_list : /* empty */ {
    
            }
            | case_list_1 {
  
};

case_list_1 : case case_list_1 {

              }
              | case {

};

case : CASE expression COLON block {

};

case_else : ELSE COLON block {

};

constructor : NEW STRING_LITERAL L_CUR colon_params_list_1 R_CUR {

              }
              | NEW STRING_LITERAL L_CUR params_list R_CUR {

};


colon_params_list : /* empty */ {   //DONE
                    
                    }
                    | colon_params_list_1 {

};

colon_params_list_1 : colon_param COMMA colon_params_list_1 {   //DONE

                      }
                      | colon_param {

};

colon_param : STRING_LITERAL COLON expression { //TODO

};

pipelines : pipeline SEMI pipelines {
            
            }
            | pipeline AND pipelines {
            
            }
            | pipeline OR pipelines {
            
            }
            | pipeline {

};

pipeline :  pipeline_1 AMPERSAND {
    
            }
            | pipeline_1 {

};

pipeline_1 :  pipeline_statement PIPE pipeline_1 {
    
              }
              | pipeline_statement {

};

pipeline_statement :  command arguments_list_1 redirections {
                      
                      }
                      | command redirections {
    
                      }
                      | command arguments_list_1 {
    
                      }
                      | command {
    
};

command : COLON STRING_LITERAL {
    
          }
          | COLON path_literal /*tuple_literal*/ {
          //tuple_literal COLON{

};

redirections :  redirections redirection {
    
                }
                | redirection {
    
};

redirection : L_COMP PATH {
    
              }
              | G_COMP PATH {

};

arguments_list_1 :  arguments_list_1 argument  {

                    }
                    | argument {

};

argument :  identifier {
            
            }
            | literal {
   
            }
         | DOT {
           
        }
           // | SLASH {
    
            //}
            | TOKEN {
            
            }
            | MINUS argument {
    
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