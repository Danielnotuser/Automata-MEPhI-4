%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "type.h"

nodeType *opr(int oper, int num_ops, ...);
nodeType *var(int i);
nodeType *num(int value);
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);

void yyerror(const char *s);
int sym[26];
%}

%union {
    int iValue;
    char sIndex;
    nodeType *nPtr;
};

%token <iValue> INTEGER
%token <sIndex> VARIABLE
%token WHILE IF PRINT
%nonassoc IFX
%nonassoc ELSE
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list

%%

program:
        function    { exit(0); }
        ;

function:
        function stmt   { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                               { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                          { $$ = $1; }
        | PRINT expr ';'                    { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'             { $$ = opr('=', 2, num($1), $3); }
        | WHILE '(' expr ')' stmt           { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX    { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt    { $$ = opr(IF, 3, $3, $5, %7); }
        | '{' stmt_list ')'                 { $$ = $2; }
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
        | expr '<' expr                 { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr                 { $$ = opr('>', 2, $1, $3); }
        | expr GE expr                  { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr                  { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr                  { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr                  { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'                  { $$ = $2; }

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

nodeType *var(int i) {
    nodeType *p;
    size_t nodeSize;

    nodeSize = SIZEOF_NODETYPE + sizeof(varNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("Out of memory.");

    p->type = typeVar;
    p->var.i = i;

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

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.num_ops; i++)
            freeNode(p->opr.op[i]);
    }
    free(p);
}

void yyerror(const char *s) {
    fprintf(stderr, "Error! %s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}
