%{
#include <stdlib.h>
#include "type.h"
#include "y.tab.h"
void yyerror(const char *);
%}

%%
[a-z]       {
                yylval.sIndex = *yytext - 'a';
                return VARIABLE;
             }

0            {
                 yylval.iValue = atoi(yytext);
                 return INTEGER;
             }

[1-9][0-9]*  {
                  yylval.iValue = atoi(yytext);
                  return INTEGER;
              }

[-()<>=+*/;{}.]  {
                    return *yytext;
                 }

">="               return GE;
"<="               return LE;
"!="               return NE;
"=="               return EQ;
"while"            return WHILE;
"if"               return IF;
"else"             return ELSE;
"print"            return PRINT;

[ \t\n]+           ;

.                  yyerror("Unknown character");
%%
int yywrap(void) {return 1}


