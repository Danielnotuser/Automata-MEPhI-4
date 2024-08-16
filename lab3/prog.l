%option case-insensitive
%{
#include <stdlib.h>
#include "type.h"
#include "y.tab.h"
void yyerror(const char *);
%}

%%
[a-z]+[0-9]*    {
                    yylval.strVal = yytext;
                    return VARIABLE;
                }

0            {
                 yylval.intVal = atoi(yytext);
                 return INTEGER;
             }

[1-9][0-9]*  {
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
"value"            return VALUE;
"pointer"          return POINTER;
"while"            return WHILE;
"finish"           return FINISH;
"break"            return BREAK;
"if"               return IF;
"else"             return ELSE;
"print"            return PRINT;

[ \t\n]+           ;

.                  yyerror("Unknown character");
%%
int yywrap(void) {return 1;}


