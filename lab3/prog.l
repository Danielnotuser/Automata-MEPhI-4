%option case-insensitive
%option yylineno
%{
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "y.tab.h"
void yyerror(const char *);
%}


%%

"value"            return VALUE;
"pointer"          return POINTER;
"while"            return WHILE;
"finish"           return FINISH;
"break"            return BREAK;
"zero?"            return ZERO;
"notzero?"         return NOTZERO;
"if"               return IF;
"else"             return ELSE;
"print"            return PRINT;

[a-z]+[0-9_]*    {
                    yylloc.last_line = yylloc.first_line = yylineno;
                    yylval.strVal = strdup(yytext);
                    return VARIABLE;
                }

0            {
                 yylloc.last_line = yylloc.first_line = yylineno;
                 yylval.intVal = atoi(yytext);
                 return INTEGER;
             }

[1-9][0-9]*  {
                  yylloc.last_line = yylloc.first_line = yylineno;
                  yylval.intVal = atoi(yytext);
                  return INTEGER;
              }

[-()<>=+*/%;{}]  {
                    return *yytext;
                 }

">="               return GE;
"<="               return LE;
"!="               return NE;
"=="               return EQ;


\n               {yylloc.last_line = yylloc.first_line = yylineno;}
[ \t]+           ;

.                {yylloc.last_line = yylloc.first_line = yylineno; yyerror("Unknown character");}
%%
int yywrap(void) {return 1;}


