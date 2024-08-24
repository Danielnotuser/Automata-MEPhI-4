%{
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <any>
#include "type.h"
#include "table.h"

VariableTable global_vars;
FunctionTable func_tab;
std::string cur_func;

extern std::istream yyin;
%}

%code requires {
  #include <iostream>
  #include <string>
  #include <vector>
  #include <map>
  #include <algorithm>
  #include <any>
  #include "type.h"
  #include "table.h"


}

%require "3.2"
%language "c++"
%locations

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error detailed

%token <int> INTEGER
%token <std::string> MAIN NAME
%token WHILE BREAK FINISH IF PRINT VALUE POINTER ZERO NOTZERO ARRAYOF CONST RETURN LPAR RPAR LSQUARE RSQUARE LBRACE RBRACE COMA SEMI ASSIGN
%nonassoc WHILEX
%nonassoc IFX
%nonassoc ELSE
%nonassoc NAMEX

%left GE LE NE EQ GREATER LESS
%left PLUS MINUS
%left STAR SLASH PERC
%nonassoc UMINUS
%nonassoc ADDR
%nonassoc DEREF


%nterm <Node*> stmt expr stmt_list init var_ref
%nterm <int> type const_type
%nterm <std::map<std::string, Variable>> args add call_args call_add

%%

program:
        main { exit(0); }
        ;

main:
        VALUE MAIN LPAR RPAR LBRACE stmt RBRACE  { Function f(0, $2, std::map<std::string,Variable>()); cur_func = $2; execute(cur_func, $6); }
        ;

functions:
          functions function
        | functions error  { std::cerr << "Error occured at line " << @2.begin.line << std::endl; yyerrok; }
        | /* NULL */
        ;

function:
        type NAME LPAR args RPAR LBRACE stmt RBRACE     { Function f($1, $2, $4); cur_func = $2; execute(cur_func, $7); }
        ;

args:
                        { $$ = std::map<std::string, Variable>(); }
      | type NAME add   { Variable v($1, $2, std::any()); std::map<std::string, Variable> res, a = $3; res.insert(std::make_pair($2,v));
                            if (!a.empty()) res.insert(a.begin(), a.end()); $$ = res; }
      ;

add:
        {}
      | COMA type NAME add { Variable v($2, $3, std::any()); std::map<std::string, Variable> res, a = $4; res.insert(std::make_pair($3,v));
                                if (!a.empty()) res.insert(a.begin(), a.end()); $$ = res; }
      ;

type:
        VALUE                                       { $$ = 0; }
        | POINTER VALUE                             { $$ = 1; }
        | ARRAYOF VALUE                             { $$ = 2; }
        | ARRAYOF POINTER VALUE                     { $$ = 3; }
        | ARRAYOF ARRAYOF VALUE                     { $$ = 4; }
        | ARRAYOF ARRAYOF POINTER VALUE             { $$ = 5; }
        ;

const_type:
          CONST VALUE                               { $$ = 6; }
        | POINTER CONST VALUE                       { $$ = 7; }
        | CONST POINTER VALUE                       { $$ = 8; }
        | CONST ARRAYOF VALUE                       { $$ = 9; }
        | CONST ARRAYOF POINTER VALUE               { $$ = 10; }
        | CONST ARRAYOF ARRAYOF VALUE               { $$ = 11; }
        ;

glob_init:
          glob_init init SEMI
        | /* NULL */
        ;

init:
          type NAME                              { std::any a; auto *v = new Variable($1, $2, a); $$ = static_cast<Node*>(v); }
        | type NAME '=' expr                     { std::any a = $4; auto *v = new Variable($1, $2, a); $$ = static_cast<Node*>(v); }
        | const_type NAME '=' expr               { std::any a = $4; auto *v = new Variable($1, $2, a); $$ = static_cast<Node*>(v); }
        ;

stmt:
          SEMI                                            { auto *op = new Operation(yy::parser::token::SEMI, 2, NULL, NULL); $$ = static_cast<Node*>(op); }
        | init SEMI                                       { $$ = $1; }
        | expr SEMI                                       { $$ = $1; }
        | RETURN expr SEMI                                { auto *op = new Operation(yy::parser::token::RETURN, 1, $2); $$ = static_cast<Node*>(op); }
        | BREAK SEMI                                      { auto *op = new Operation(yy::parser::token::BREAK, 0); $$ = static_cast<Node*>(op); }
        | var_ref ASSIGN expr SEMI                        { auto *op = new Operation(yy::parser::token::ASSIGN, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | PRINT expr SEMI                                 { auto *op = new Operation(yy::parser::token::PRINT, 1, $2); $$ = static_cast<Node*>(op); }
        | WHILE LPAR expr RPAR stmt %prec WHILEX          { auto *op = new Operation(yy::parser::token::WHILE, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | WHILE LPAR expr RPAR stmt FINISH stmt           { auto *op = new Operation(yy::parser::token::WHILE, 3, $3, $5, $7); $$ = static_cast<Node*>(op); }
        | IF LPAR expr RPAR stmt %prec IFX                { auto *op = new Operation(yy::parser::token::IF, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | IF LPAR expr RPAR stmt ELSE stmt                { auto *op = new Operation(yy::parser::token::IF, 3, $3, $5, $7); $$ = static_cast<Node*>(op); }
        | ZERO LPAR expr RPAR stmt                        { auto *op = new Operation(yy::parser::token::ZERO, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | NOTZERO LPAR expr RPAR stmt                     { auto *op = new Operation(yy::parser::token::NOTZERO, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | LBRACE stmt_list RBRACE                         { $$ = $2; }
        ;

stmt_list:
          stmt                { $$ = $1; }
        | stmt_list stmt      { auto *op = new Operation(';', 2, $1, $2); }
        ;

var_ref:
        NAME %prec NAMEX    { Variable *v = new Variable; if (!global_vars.find_var($1, *v) && !func_tab.find_var($1, cur_func, *v))
                                                 std::cerr << "Invalid access to an uninitialized variable.";
                                $$ = static_cast<Node*>(v);
                            }
        ;

expr:
          INTEGER                               { auto *n = new Number($1); $$ = static_cast<Node*>(n); }
        | var_ref                               { $$ = $1; }
        | NAME LPAR call_args RPAR              { auto *n = new Number; $$ = static_cast<Node*>(n); }
        | NAME LSQUARE expr RSQUARE             { auto *op = new Operation(yy::parser::token::LSQUARE, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | STAR var_ref %prec DEREF              { auto *op = new Operation(yy::parser::token::DEREF, 1, $2); $$ = static_cast<Node*>(op); }
        | MINUS expr %prec UMINUS               { auto *op = new Operation(yy::parser::token::UMINUS, 1, $2); $$ = static_cast<Node*>(op); }
        | ADDR var_ref                          { auto *op = new Operation(yy::parser::token::ADDR, 1, $2); $$ = static_cast<Node*>(op); }
        | expr PLUS expr                        { auto *op = new Operation(yy::parser::token::PLUS, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr MINUS expr                       { auto *op = new Operation(yy::parser::token::MINUS, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr STAR expr                        { auto *op = new Operation(yy::parser::token::STAR, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr SLASH expr                       { auto *op = new Operation(yy::parser::token::SLASH, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr PERC expr                        { auto *op = new Operation(yy::parser::token::PLUS, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr LESS expr                        { auto *op = new Operation(yy::parser::token::LESS, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr GREATER expr                     { auto *op = new Operation(yy::parser::token::GREATER, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr GE expr                          { auto *op = new Operation(yy::parser::token::GE, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr LE expr                          { auto *op = new Operation(yy::parser::token::LE, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr NE expr                          { auto *op = new Operation(yy::parser::token::NE, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr EQ expr                          { auto *op = new Operation(yy::parser::token::EQ, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | LPAR expr RPAR                        { $$ = $2; }
        ;

call_args:
        { $$ = std::map<std::string, Variable>(); }
      | type NAME call_add   { Variable v($1, $2, std::any());
                            std::map<std::string, Variable> res, a = $3; res.insert(std::make_pair($2,v));
                            if (!a.empty()) res.insert(a.begin(), a.end()); $$ = res; }
      ;

call_add:
        { $$ = std::map<std::string, Variable>(); }
      | COMA type NAME call_add { Variable v($2, $3, std::any());
                               std::map<std::string, Variable> res, a = $4; res.insert(std::make_pair($3,v));
                               if (!a.empty()) res.insert(a.begin(), a.end()); $$ = res; }
      ;
%%

namespace yy
{
  // Report an error to the user.
  auto parser::error (const location_type& l, const std::string& m) -> void
  {
    std::cerr << "Error : location: " << l << ", message: " << m << '\n';
  }
}

int main(int argc, const char *argv[])
{
    if (argc >= 2) {std::ifstream f(argv[1]); std::istream &yyin = static_cast<::std::istream&>(f);}
    else std::istream &yyin = static_cast<::std::istream&>(::std::cin);
    yy::parser my_parse;
    return my_parse.parse();;
}
