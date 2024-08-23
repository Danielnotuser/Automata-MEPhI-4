%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "type.h"

nodeType *opr(int oper, int num_ops, ...);
nodeType *var(char *str);
nodeType *num(int value);
void freeNode(nodeType *p);
void freeMap();
int ex(nodeType *p);
int yylex(void);

void yyerror(const char *s);
VarMap global_vars
extern FILE *yyin;
%}

%union {
    int intVal;
    char *strVal;
    struct nodeTypeTag *nPtr;
}

%token <intVal> INTEGER
%token <strVal> VARIABLE
%token WHILE BREAK FINISH IF PRINT VALUE POINTER ZERO NOTZERO ARRAYOF
%nonassoc WHILEX
%nonassoc IFX
%nonassoc ELSE

%left GE LE NE EQ '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS
%nonassoc USTAR

%type <nPtr> stmt expr stmt_list
%type <intVal> type

%%

program:
        function      { freeMap(); exit(0); }
        ;

function:
        function stmt     { ex($2); freeNode($2); }
        | function error  { printf("Error occured at line %d\n", @2.first_line); yyerrok; }
        | /* NULL */
        ;

type:
        VALUE                                       { $$ = 0; }
        | POINTER VALUE                             { $$ = 1; }
        | POINTER CONST VALUE                       { $$ = 2; }
        | ARRAYOF VALUE                             { $$ = 3; }
        | ARRAYOF CONST VALUE                       { $$ = 4; }
        | ARRAYOF POINTER VALUE                     { $$ = 5; }
        | ARRAYOF CONST POINTER VALUE               { $$ = 6; }
        | ARRAYOF POINTER CONST VALUE               { $$ = 7; }
        | ARRAYOF ARRAYOF VALUE                     { $$ = 8; }
        | ARRAYOF CONST ARRAYOF VALUE               { $$ = 9; }
        | ARRAYOF ARRAOF CONST VALUE                { $$ = 10; }
        | ARRAYOF ARRAYOF POINTER VALUE             { $$ = 11; }
        | ARRAYOF ARRAYOF CONST POINTER VALUE       { $$ = 12; }
        | ARRAYOF ARRAYOF POINTER CONST VALUE       { $$ = 13; }
        ;
const_type:
        | CONST VALUE                               { $$ = 21; }
        | CONST POINTER VALUE                       { $$ = 14; }
        | CONST ARRAYOF VALUE                       { $$ = 15; }
        | CONST ARRAYOF POINTER VALUE               { $$ = 16; }
        | CONST ARRAYOF POINTER CONST VALUE         { $$ = 17; }
        | CONST ARRAYOF CONST POINTER VALUE         { $$ = 18; }
        | CONST ARRAYOF ARRAYOF VALUE               { $$ = 19; }
        | CONST ARRAYOF CONST ARRAYOF VALUE         { $$ = 20; }
        ;

stmt:
          ';'                                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                                       { $$ = $1; }
        | BREAK ';'                                      { $$ = opr(BREAK, 0); }
        | type VARIABLE ';'                              { $$ = opr(VALUE, 3, num($1), var($2), NULL); }
        | type VARIABLE '=' expr ';'                     { $$ = opr(VALUE, 3, num($1), var($2), $4); }
        | const_type VARIABLE '=' expr ';'               { $$ = opr(VALUE, 3, num($1), var($2), $4); }
        | VARIABLE '=' expr ';'                          { $$ = opr('=', 2, var($1), $3); }
        | PRINT expr ';'                                 { $$ = opr(PRINT, 1, $2); }
        | WHILE '(' expr ')' stmt %prec WHILEX           { $$ = opr(WHILE, 2, $3, $5); }
        | WHILE '(' expr ')' stmt FINISH stmt            { $$ = opr(WHILE, 3, $3, $5, $7); }
        | IF '(' expr ')' stmt %prec IFX                 { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt                 { $$ = opr(IF, 3, $3, $5, $7); }
        | ZERO '(' expr ')' stmt                         { $$ = opr(ZERO, 2, $3, $5); }
        | NOTZERO '(' expr ')' stmt                      { $$ = opr(NOTZERO, 2, $3, $5); }
        | '{' stmt_list '}'                              { $$ = $2; }
        ;

stmt_list:
          stmt                { $$ = $1; }
        | stmt_list stmt      { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER                       { $$ = num($1); }
        | VARIABLE                      { $$ = var($1); }
        | '-' expr %prec UMINUS         { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr                 { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr                 { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr                 { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr                 { $$ = opr('/', 2, $1, $3); }
        | expr '%' expr                 { $$ = opr('%', 2, $1, $3); }
        | expr '<' expr                 { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr                 { $$ = opr('>', 2, $1, $3); }
        | expr GE expr                  { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr                  { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr                  { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr                  { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'                  { $$ = $2; }
        ;

%%

#define SIZEOF_NODETYPE ((char *)&p->num - (char *)p)

nodeType *num(int value) {
    nodeType *p;
    size_t nodeSize;

    nodeSize = SIZEOF_NODETYPE + sizeof(numNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("Out of memory.");

    p->type = typeNum;
    p->num.value = value;

    return p;
}

nodeType *var(char *str) {
    nodeType *p;
    size_t nodeSize;

    nodeSize = SIZEOF_NODETYPE + sizeof(varNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("Out of memory.");

    p->type = typeVar;
    p->var.name = str;

    return p;
}




nodeType *opr(int oper, int num_ops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
                (num_ops - 1) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("Out of memory.");

    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.num_ops = num_ops;
    va_start(ap, num_ops);
    for (i = 0; i < num_ops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.num_ops; i++)
            freeNode(p->opr.op[i]);
    }
    else if (p->type == typeVar)
        free(p->var.name);
    free(p);
}

void freeMap() {
    free(vars.var_value);
    for (int i = 0; i < vars.var_num; i++)
        free(vars.var_name[i]);
    free(vars.var_name);
}

void yyerror(const char *s) {
    fprintf(stderr, "Error! %s\n", s);
}

int main(int argc, const char *argv[]) {
    if (argc >= 2) yyin = fopen(argv[1], "r");
    if (!yyin) yyin = stdin;
    yyparse();
    return 0;
}
