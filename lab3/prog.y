%{
#define YYSTYPE val
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <any>
#include "type.h"
#include "table.h"

struct val  {
	char* strVal;
	int intVal;
	Node* nPtr;
	VariableTable argArr;
};

VariableTable global_vars;
FunctionTable func_tab;
std::string cur_func;

int yylex();
void yyerror(const char*);

extern int yylineno;
extern FILE *yyin;
%}

%token INTEGER
%token MAIN NAME
%token WHILE BREAK IF PRINT VALUE POINTER ZERO NOTZERO ARRAYOF CONST RETURN LPAR RPAR LSQUARE RSQUARE LBRACE RBRACE COMA SEMI ASSIGN
%nonassoc WHILEX
%nonassoc FINISH
%nonassoc IFX
%nonassoc ELSE
%nonassoc NAMEX

%left GE LE NE EQ GREATER LESS
%left PLUS MINUS
%left STAR SLASH PERC
%nonassoc UMINUS
%nonassoc ADDR
%nonassoc DEREF


%nterm stmt expr stmt_list init var_ref glob_init functions function main
%nterm type const_type
%nterm args add

%%

program:
          main                        { exit(0); }
        | functions main              { exit(0); }
        | glob_init main              { exit(0); }
        | glob_init functions main    { exit(0); }
        ;

main_name:
        VALUE MAIN LPAR RPAR { cur_func = $2.strVal; Function f(0, $2.strVal, VariableTable()); func_tab.insert_func(f); }
        ;

main:
        main_name stmt      {  Function f; func_tab.find_func(cur_func, f); f.set_ptr($2.nPtr); f.execute(); }
        ;

functions:
          function
        | functions function
        | functions error           { std::cerr << "Error occured at line " << @2.first_line << std::endl; yyerrok; }
        ;

function_name:
        type NAME LPAR args RPAR    { cur_func = $2.strVal; Function f($1.intVal, $2.strVal, $4.argArr); func_tab.insert_func(f); }
        ;

function:
        function_name stmt          { Function f; func_tab.find_func(cur_func, f); f.set_ptr($2.nPtr); f.execute(); }
        ;

args:
                        { $$.argArr = VariableTable(); }
      | type NAME add   { Variable v($1.intVal, $2.strVal); VariableTable res, a = $3.argArr; res.insert(v);
                            if (!a.empty()) res.insert(a); $$.argArr = res; }
      ;

add:
                            { $$.argArr = VariableTable(); }
      | COMA type NAME add  { Variable v($2.intVal, $3.strVal); VariableTable res, a = $4.argArr; res.insert(v);
                                if (!a.empty()) res.insert(a); $$.argArr = res; }
      ;

type:
        VALUE                                       { $$.intVal = 0; }
        | POINTER VALUE                             { $$.intVal = 1; }
        | ARRAYOF VALUE                             { $$.intVal = 2; }
        | ARRAYOF POINTER VALUE                     { $$.intVal = 3; }
        | ARRAYOF ARRAYOF VALUE                     { $$.intVal = 4; }
        | ARRAYOF ARRAYOF POINTER VALUE             { $$.intVal = 5; }
        ;

const_type:
          CONST VALUE                               { $$.intVal = 6; }
        | POINTER CONST VALUE                       { $$.intVal = 7; }
        | CONST POINTER VALUE                       { $$.intVal = 8; }
        | CONST ARRAYOF VALUE                       { $$.intVal = 9; }
        | CONST ARRAYOF POINTER VALUE               { $$.intVal = 10; }
        | CONST ARRAYOF ARRAYOF VALUE               { $$.intVal = 11; }
        ;

glob_init:
           init SEMI
        |  glob_init init SEMI
        ;

init:
          type NAME                                 { auto *v = new Variable($1.intVal, $2.strVal);
                                                        if (func_tab.find_var(cur_func, $2.strVal, *v)) { std::cerr << "Redefinition of the variable " << $2.strVal << ".\n";  $$.nPtr = static_cast<Node*>(v); }
                                                        else {  auto *op = new Operation(VALUE, 1, static_cast<Node*>(v)); func_tab.insert_var(cur_func, *v); $$.nPtr = static_cast<Node*>(op); } }
        | type NAME ASSIGN expr                     { auto *v = new Variable($1.intVal, $2.strVal);
                                                        if (func_tab.find_var(cur_func, $2.strVal, *v)) { std::cerr << "Redefinition of the variable " << $2.strVal << ".\n"; $$.nPtr = static_cast<Node*>(v); }
                                                        else { auto *op = new Operation(VALUE, 2, static_cast<Node*>(v), $4.nPtr); func_tab.insert_var(cur_func, *v); $$.nPtr = static_cast<Node*>(op); } }
        | const_type NAME ASSIGN expr               { auto *v = new Variable($1.intVal, $2.strVal);
                                                        if (func_tab.find_var(cur_func, $2.strVal, *v)) { std::cerr << "Redefinition of the variable " << $2.strVal << ".\n";  $$.nPtr = static_cast<Node*>(v); }
                                                        else { auto *op = new Operation(VALUE, 2, static_cast<Node*>(v), $4.nPtr); func_tab.insert_var(cur_func, *v); $$.nPtr = static_cast<Node*>(op); } }
        ;

stmt:
          SEMI                                                               { auto *op = new Operation(SEMI, 2, NULL, NULL); $$.nPtr = static_cast<Node*>(op); }
        | init SEMI                                                          { $$.nPtr = $1.nPtr; }
        | expr SEMI                                                          { $$.nPtr = $1.nPtr; }
        | RETURN expr SEMI                                                   { auto *op = new Operation(RETURN, 1, $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | BREAK SEMI                                                         { auto *op = new Operation(BREAK, 0); $$.nPtr = static_cast<Node*>(op); }
        | var_ref ASSIGN expr SEMI                                           { auto *op = new Operation(ASSIGN, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | PRINT expr SEMI                                                    { auto *op = new Operation(PRINT, 1, $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | WHILE LPAR expr RPAR stmt %prec WHILEX                             { auto *op = new Operation(WHILE, 2, $3.nPtr, $5.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | WHILE LPAR expr RPAR stmt FINISH stmt                              { auto *op = new Operation(WHILE, 3, $3.nPtr, $5.nPtr, $7.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | IF LPAR expr RPAR stmt %prec IFX                                   { auto *op = new Operation(IF, 2, $3.nPtr, $5.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | IF LPAR expr RPAR stmt ELSE stmt                                   { auto *op = new Operation(IF, 3, $3.nPtr, $5.nPtr, $7.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | ZERO LPAR expr RPAR stmt                                           { auto *op = new Operation(ZERO, 2, $3.nPtr, $5.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | NOTZERO LPAR expr RPAR stmt                                        { auto *op = new Operation(NOTZERO, 2, $3.nPtr, $5.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | LBRACE stmt_list RBRACE                                            { $$.nPtr = $2.nPtr; }
        ;

stmt_list:
          stmt                { $$.nPtr = $1.nPtr; }
        | stmt_list stmt      { auto *op = new Operation(SEMI, 2, $1.nPtr, $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        ;

var_ref:
        NAME %prec NAMEX    { Variable *v = new Variable; if (!func_tab.find_var(cur_func, $1.strVal, *v) && !global_vars.find_var($1.strVal, *v))
                                                 std::cerr << "Error! Invalid access to an uninitialized variable. line: " << @1.first_line << "\n";
                                $$.nPtr = static_cast<Node*>(v);
                            }
        ;

expr:
          INTEGER                               { auto *n = new Number($1.intVal); $$.nPtr = static_cast<Node*>(n); }
        | var_ref                               { $$.nPtr = $1.nPtr; }
//        | NAME LPAR call_args RPAR              { auto *n = new Number; $$.nPtr = static_cast<Node*>(n); }
        | NAME LSQUARE expr RSQUARE             { auto *op = new Operation(LSQUARE, 2, $1.strVal, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | STAR var_ref %prec DEREF              { auto *op = new Operation(DEREF, 1, $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | MINUS expr %prec UMINUS               { auto *op = new Operation(UMINUS, 1, $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | ADDR var_ref                          { auto *op = new Operation(ADDR, 1, $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr PLUS expr                        { auto *op = new Operation(PLUS, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr MINUS expr                       { auto *op = new Operation(MINUS, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr STAR expr                        { auto *op = new Operation(STAR, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr SLASH expr                       { auto *op = new Operation(SLASH, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr PERC expr                        { auto *op = new Operation(PLUS, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr LESS expr                        { auto *op = new Operation(LESS, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr GREATER expr                     { auto *op = new Operation(GREATER, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr GE expr                          { auto *op = new Operation(GE, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr LE expr                          { auto *op = new Operation(LE, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr NE expr                          { auto *op = new Operation(NE, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr EQ expr                          { auto *op = new Operation(EQ, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | LPAR expr RPAR                        { $$.nPtr = $2.nPtr; }
        ;

/*
call_args:
        { $$.argArr = VariableTable(); }
      | var_ref call_add   { Variable *v = static_cast<Variable*>($1.nPtr);
                               VariableTable res, a = $2.argArr; res.insert(*v);
                               if (!a.empty()) res.insert(a); $$.argArr = res; }
      ;

call_add:
        { $$.argArr = VariableTable(); }
      | COMA var_ref call_add { Variable v($2.intVal, $3.strVal, std::any());
                               VariableTable res, a = $4.argArr; res.insert(*v);
                               if (!a.empty()) res.insert(a); $$.argArr = res; }
      ;
*/

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error occured at %d line: %s\n", yylineno, s);
}


int main(int argc, const char *argv[])
{
    if (argc >= 2) yyin = fopen(argv[1], "r");
    if (!yyin) yyin = stdin;
    yyparse();
    return 0;
}
