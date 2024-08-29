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
#include "robot.h"

struct val  {
	char* strVal;
	int intVal;
	Node* nPtr;
	VariableTable argArr;
	NodeArr argCall;
};

VariableTable glob_vars;
FunctionTable func_tab;
Robot rob("maze.txt");
std::string cur_func;

int yylex();
void yyerror(const char*);

extern int yylineno;
extern FILE *yyin;
%}

%token INTEGER
%token MAIN NAME
%token WHILE BREAK IF PRINT VALUE POINTER ZERO NOTZERO ARRAYOF CONST RETURN LPAR RPAR LSQUARE RSQUARE LBRACE RBRACE COMA SEMI ASSIGN FUNC SIZE
%token TOP BOTTOM LEFT RIGHT PORTAL TELEPORT
%nonassoc WHILEX
%nonassoc FINISH
%nonassoc IFX
%nonassoc ELSE
%nonassoc NAMEX
%nonassoc ASDER
%nonassoc ASARR

%left GE LE NE EQ GREATER LESS
%left PLUS MINUS
%left STAR SLASH PERC
%nonassoc UMINUS
%nonassoc ADDR
%nonassoc DEREF


%nterm var_ref stmt expr stmt_list
%nterm init init_list glob_init
%nterm functions function main
%nterm type const_type
%nterm args add

%%

program:
          main                        { exit(0); }
        | functions main              { exit(0); }
        | init_list main              { exit(0); }
        | init_list functions main    { exit(0); }
        | error                       { std::cerr << "Error occured at line " << @1.first_line << std::endl; yyerrok; exit(0); }
        ;

main_name:
        VALUE MAIN LPAR RPAR { cur_func = $2.strVal; Function f(0, $2.strVal, VariableTable()); func_tab.insert_func(f); }
        ;

main:
        main_name stmt      {  Function f; func_tab.find_func(cur_func, &f); f.set_ptr($2.nPtr); f.execute(); }
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
        function_name stmt          { Function f; func_tab.find_func(cur_func, &f); f.set_ptr($2.nPtr); }
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
        | CONST ARRAYOF ARRAYOF POINTER VALUE       { $$.intVal = 12; }
        ;

init_list:
           glob_init                 { Function glob_scope; glob_scope.set_ptr($1.nPtr); glob_scope.execute();
                                        VariableTable temp = glob_scope.get_tab(); glob_vars.insert(temp); }
        |  init_list glob_init       { auto *op = new Operation(SEMI, 2, $1.nPtr, $2.nPtr); Function glob_scope; glob_scope.set_ptr(static_cast<Node*>(op));
                                        glob_scope.execute(); VariableTable temp = glob_scope.get_tab(); glob_vars.insert(temp); }
        ;

glob_init:
          type NAME SEMI                               { auto *v = new Variable($1.intVal, $2.strVal);
                                                        auto *op = new Operation(&glob_vars, VALUE, 1, static_cast<Node*>(v)); $$.nPtr = static_cast<Node*>(op); }
        | type NAME ASSIGN expr SEMI                   { auto *v = new Variable($1.intVal, $2.strVal);
                                                        auto *op = new Operation(&glob_vars, VALUE, 2, static_cast<Node*>(v), $4.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | const_type NAME ASSIGN expr SEMI             { auto *v = new Variable($1.intVal, $2.strVal);
                                                        auto *op = new Operation(&glob_vars, VALUE, 2, static_cast<Node*>(v), $4.nPtr); $$.nPtr = static_cast<Node*>(op); }

init:
          type NAME SEMI                               { auto *v = new Variable($1.intVal, $2.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                        auto *op = new Operation(&f, VALUE, 1, static_cast<Node*>(v)); $$.nPtr = static_cast<Node*>(op); }
        | type NAME ASSIGN expr SEMI                   { auto *v = new Variable($1.intVal, $2.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                        auto *op = new Operation(&f, VALUE, 2, static_cast<Node*>(v), $4.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | const_type NAME ASSIGN expr SEMI             { auto *v = new Variable($1.intVal, $2.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                        auto *op = new Operation(&f, VALUE, 2, static_cast<Node*>(v), $4.nPtr); $$.nPtr = static_cast<Node*>(op); }
        ;

stmt:
          SEMI                                                               { auto *op = new Operation(SEMI, 0); $$.nPtr = static_cast<Node*>(op); }
        | init                                                               { $$.nPtr = $1.nPtr; }
        | expr SEMI                                                          { $$.nPtr = $1.nPtr; }
        | RETURN expr SEMI                                                   { Function f; func_tab.find_func(cur_func, &f); Number *n = new Number(f.get_ret_type());
                                                                                auto *op = new Operation(RETURN, 2, static_cast<Node*>(n), $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | BREAK SEMI                                                         { auto *op = new Operation(BREAK, 0); $$.nPtr = static_cast<Node*>(op); }
        | NAME ASSIGN expr SEMI                                              { Variable *v = new Variable(0, $1.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                                               auto *op = new Operation(&glob_vars, &f, ASSIGN, 2, static_cast<Node*>(v), $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | STAR NAME ASSIGN expr SEMI %prec ASDER                             { Variable *v = new Variable(0, $2.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                                               auto *op = new Operation(&glob_vars, &f, ASDER, 2, static_cast<Node*>(v), $4.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | NAME LSQUARE expr RSQUARE ASSIGN expr SEMI  %prec ASARR            { Variable *v = new Variable(0, $1.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                                               auto *op = new Operation(&glob_vars, &f, ASARR, 3, static_cast<Node*>(v), $3.nPtr, $6.nPtr); $$.nPtr = static_cast<Node*>(op); }
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
        NAME %prec NAMEX    { Variable *v = new Variable(0, $1.strVal); VariableTable f = func_tab.get_tab(cur_func);
                            auto *op = new Operation(&glob_vars, &f, NAME, 1, static_cast<Node*>(v)); $$.nPtr = static_cast<Node*>(op); }
        ;

expr:
          INTEGER                               { auto *n = new Number($1.intVal); $$.nPtr = static_cast<Node*>(n); }
        | var_ref                               { $$.nPtr = $1.nPtr; }
        | call_func                             { $$.nPtr = $1.nPtr; }
        | NAME LSQUARE expr RSQUARE             { Variable *v = new Variable(0, $1.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                  auto *op = new Operation(&glob_vars, &f, LSQUARE, 2, static_cast<Node*>(v), $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | STAR NAME %prec DEREF                 { Variable *v = new Variable(0, $2.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                  auto *op = new Operation(&glob_vars, &f, DEREF, 1, static_cast<Node*>(v)); $$.nPtr = static_cast<Node*>(op); }
        | ADDR NAME                             { Variable *v = new Variable(0, $2.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                  auto *op = new Operation(&glob_vars, &f, ADDR, 1, static_cast<Node*>(v)); $$.nPtr = static_cast<Node*>(op); }
        | MINUS expr %prec UMINUS               { auto *op = new Operation(UMINUS, 1, $2.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr PLUS expr                        { auto *op = new Operation(PLUS, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr MINUS expr                       { auto *op = new Operation(MINUS, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr STAR expr                        { auto *op = new Operation(STAR, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr SLASH expr                       { auto *op = new Operation(SLASH, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr PERC expr                        { auto *op = new Operation(PERC, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr LESS expr                        { auto *op = new Operation(LESS, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr GREATER expr                     { auto *op = new Operation(GREATER, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr GE expr                          { auto *op = new Operation(GE, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr LE expr                          { auto *op = new Operation(LE, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr NE expr                          { auto *op = new Operation(NE, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | expr EQ expr                          { auto *op = new Operation(EQ, 2, $1.nPtr, $3.nPtr); $$.nPtr = static_cast<Node*>(op); }
        | SIZE NAME                             { Variable *v = new Variable(0, $2.strVal); VariableTable f = func_tab.get_tab(cur_func);
                                                  auto *op = new Operation(&glob_vars, &f, SIZE, 1, static_cast<Node*>(v)); $$.nPtr = static_cast<Node*>(op); }
        | robot                                 { $$.nPtr = $1.nPtr; }
        | LPAR expr RPAR                        { $$.nPtr = $2.nPtr; }
        ;

call_func:
          NAME LPAR call_args RPAR              { VariableTable ftab = func_tab.get_tab(cur_func);
                                                  Function f; if (!func_tab.find_func($1.strVal, &f)) std::cerr << "Reference to the unimplemented function." << std::endl;
                                                  auto *op = new Operation(&f, &glob_vars, &ftab, FUNC, $3.argCall); $$.nPtr = static_cast<Node*>(op); }
         ;
call_args:
        { $$.argCall = NodeArr(); }
      | var_ref call_add   { NodeArr res, a = $2.argCall; res.nodes.push_back($1.nPtr);
                             if (!a.nodes.empty()) res.nodes.insert(res.nodes.end(), a.nodes.begin(), a.nodes.end()); $$.argCall = res; }
      ;

call_add:
        { $$.argArr = VariableTable(); }
      | COMA var_ref call_add { NodeArr res, a = $3.argCall; res.nodes.push_back($2.nPtr);
                                if (!a.nodes.empty()) res.nodes.insert(res.nodes.end(), a.nodes.begin(), a.nodes.end()); $$.argCall = res; }
      ;

robot:
          TOP            { auto *op = new Operation(&rob, TOP); $$.nPtr = static_cast<Node*>(op); }
        | BOTTOM         { auto *op = new Operation(&rob, BOTTOM); $$.nPtr = static_cast<Node*>(op); }
        | LEFT           { auto *op = new Operation(&rob, LEFT); $$.nPtr = static_cast<Node*>(op); }
        | RIGHT          { auto *op = new Operation(&rob, RIGHT); $$.nPtr = static_cast<Node*>(op); }
        | PORTAL         { auto *op = new Operation(&rob, PORTAL); $$.nPtr = static_cast<Node*>(op); }
        | TELEPORT       { auto *op = new Operation(&rob, TELEPORT); $$.nPtr = static_cast<Node*>(op); }
        ;





%%

void yyerror(const char *s) {
    fprintf(stderr, "Error occured at %d line: %s\n", yylineno, s);
}


int main(int argc, const char *argv[])
{
    if (argc >= 2)  yyin = fopen(argv[1], "r");
    else yyin = stdin;
    try {yyparse();}
    catch (const int &ret) { return ret; }
    return 0;
}
