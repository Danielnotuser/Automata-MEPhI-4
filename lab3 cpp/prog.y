%{
#include <iostream>
#include <string>
#include <algorithm>
#include <any>
#include "type.h"
#include "table.h"

Table<Variable> global_vars;
Table<Function> func_tab;
std::string cur_func;

int ex(nodeType *p);
int yylex(void);

void yyerror(const char *s);
extern FILE *yyin;
%}


%union {
    int intVal;
    std::string strVal;
    std::vector<std::pair<int, std::string>> argArr;
    Node *nPtr;
}

%token <intVal> INTEGER
%token <strVal> NAME
%token WHILE BREAK FINISH IF PRINT VALUE POINTER ZERO NOTZERO ARRAYOF
%nonassoc WHILEX
%nonassoc IFX
%nonassoc ELSE
%nonassoc NAMEX

%left GE LE NE EQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS
%nonassoc USTAR
%nonassoc ADDR
%nonassoc DEREF

%type <nPtr> stmt expr stmt_list init var_ref
%type <intVal> type
%type <argArr> args add call_args call_add

%%

program:
        glob_init functions main  { freeMap(); exit(0); }
        ;

main:
        VALUE MAIN '(' ')' '{' stmt '}'     { Function f($1, $2, std::map<std::string,Variable>()); cur_func = $2; execute(cur_func, $6); freeNode($2); }
        ;

functions:
          %empty
        | functions function
        | functions error  { std::cerr << "Error occured at line " << @2.first_line << std::endl; yyerrok; }
        | /* NULL */
        ;

function:
        type NAME '(' args ')' '{' stmt '}'     { Function f($1, $2, $3); cur_func = $2; execute(cur_func, $7); freeNode($2); }
        ;

args:
        %empty          { $$ = std::map<std::string, Variable>(); }
      | type NAME add   { Variable v($1, $2, std::any()); std::map<std::string, Variable> res = {make_pair($2,v)}, a = $3; if (!a.empty()) res.insert(std::next(res.begin(), 1), a.begin(), a.end()); $$ = res; }
      ;

add:
        %empty
      | ',' type NAME add { Variable v($2, $3, std::any()); std::map<std::string, Variable> res = {make_pair($3,v)}, a = $4; if (!a.empty()) res.insert(std::next(res.begin(), 1), a.begin(), a.end()); $$ = res; }
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
        | CONST VALUE                               { $$ = 6; }
        | POINTER CONST VALUE                       { $$ = 7; }
        | CONST POINTER VALUE                       { $$ = 8; }
        | CONST ARRAYOF VALUE                       { $$ = 9; }
        | CONST ARRAYOF POINTER VALUE               { $$ = 10; }
        | CONST ARRAYOF ARRAYOF VALUE               { $$ = 11; }
        ;

glob_init:
        %empty
        | init
        ;

init:
        | type NAME ';'                              { std::any a; auto *v = new Variable($1, $2, a); $$ = static_cast<Node*>(v); }
        | type NAME '=' expr ';'                     { std::any a = $4; auto *v = new Variable($1, $2, a); $$ = static_cast<Node*>(v); }
        | const_type VARIABLE '=' expr ';'           { std::any a = $4; auto *v = new Variable($1, $2, a); $$ = static_cast<Node*>(v); }
        ;

stmt:
          ';'                                            { auto *op = new Operation(';', 2, NULL, NULL); $$ = static_cast<Node*>(op); }
        | init                                           { $$ = $1; }
        | expr ';'                                       { $$ = $1; }
        | RETURN expr ';'                                { auto *op = new Operation(RETURN, 1, $2); $$ = static_cast<Node*>(op); }
        | BREAK ';'                                      { auto *op = new Operation(BREAK, 0); $$ = static_cast<Node*>(op); }
        | VARIABLE '=' expr ';'                          { auto *op = new Operation('=', 2, var($1), $3); $$ = static_cast<Node*>(op); }
        | PRINT expr ';'                                 { auto *op = new Operation(PRINT, 1, $2); $$ = static_cast<Node*>(op); }
        | WHILE '(' expr ')' stmt %prec WHILEX           { auto *op = new Operation(WHILE, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | WHILE '(' expr ')' stmt FINISH stmt            { auto *op = new Operation(WHILE, 3, $3, $5, $7); $$ = static_cast<Node*>(op); }
        | IF '(' expr ')' stmt %prec IFX                 { auto *op = new Operation(IF, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | IF '(' expr ')' stmt ELSE stmt                 { auto *op = new Operation(IF, 3, $3, $5, $7); $$ = static_cast<Node*>(op); }
        | ZERO '(' expr ')' stmt                         { auto *op = new Operation(ZERO, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | NOTZERO '(' expr ')' stmt                      { auto *op = new Operation(NOTZERO, 2, $3, $5); $$ = static_cast<Node*>(op); }
        | '{' stmt_list '}'                              { $$ = $2; }
        ;

stmt_list:
          stmt                { $$ = $1; }
        | stmt_list stmt      { auto *op = new Operation(';', 2, $1, $2); }
        ;

var_ref:
        | NAME %prec NAMEX    { Variable v; if (!global_vars.find_var($1, v) && !func_tab.find_var($1, cur_func, v))
                                                 std::cerr << "Invalid access to an uninitialized variable.";
                                $$ = static_cast<Node*>(v);
                              }
        ;

expr:
          INTEGER                       { auto *n = new Number($1); $$ = static_cast<Node*>(n); }
        | var_ref                       { $$ = $1; }
        | NAME '(' call_args ')'        { $$ = call_func($1, $3); }
        | NAME '[' expr ']'             { auto *op = new Operation('[', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | '*' var_ref %prec DEREF       { auto *op = new Operation(DEREF, 1, $2); $$ = static_cast<Node*>(op); }
        | '-' expr %prec UMINUS         { auto *op = new Operation(UMINUS, 1, $2); $$ = static_cast<Node*>(op); }
        | '&' var_ref %prec ADDR        { auto *op = new Operation(ADDR, 1, $2); $$ = static_cast<Node*>(op); }
        | expr '+' expr                 { auto *op = new Operation('+', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr '-' expr                 { auto *op = new Operation('-', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr '*' expr                 { auto *op = new Operation('*', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr '/' expr                 { auto *op = new Operation('/', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr '%' expr                 { auto *op = new Operation('%', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr '<' expr                 { auto *op = new Operation('<', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr '>' expr                 { auto *op = new Operation('>', 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr GE expr                  { auto *op = new Operation(GE, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr LE expr                  { auto *op = new Operation(LE, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr NE expr                  { auto *op = new Operation(NE, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | expr EQ expr                  { auto *op = new Operation(EQ, 2, $1, $3); $$ = static_cast<Node*>(op); }
        | '(' expr ')'                  { $$ = $2; }
        ;

call_args:
        %empty
      | type NAME call_add   { argArr res = {make_pair($1,$2)}, a = $3; if (!a.empty()) res.insert(std::next(res.begin(), 1), a.begin(), a.end()); $$ = res; }
      ;

call_add:
        %empty
      | ',' type NAME call_add { argArr res = {make_pair($2,$3)}, a = $4; if (!a.empty()) res.insert(std::next(res.begin(), 1), a.begin(), a.end()); $$ = res; }
      ;
%%

Variable find_var(std::string name)
{
    if (auto f = global_vars.find(name); f != global_vars.end())
        return f;
    if (auto f = func_tab[cur_func].vars.find(name); f != func_tab[cur_func].vars.end())
        return f;
    return Variable();
}

void yyerror(const char *s) {
    fprintf(stderr, "Error! %s\n", s);
}

int main(int argc, const char *argv[]) {
    if (argc >= 2) yyin = fopen(argv[1], "r");
    if (!yyin) yyin = stdin;
    try
        yyparse();
   catch (std::exception &e)
        std::cout << e.what() << std::endl;
    return 0;
}
