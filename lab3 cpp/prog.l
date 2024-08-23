%option case-insensitive
%option noyywrap
%option c++
%option yylineno
%{
#include <iostream>
#include <string>
#include "type.h"
#include "y.tab.h"
void yyerror(const char *);
%}


%%

"value"            return VALUE;
"pointer"          return POINTER;
"array of"         return ARRAYOF;
"while"            return WHILE;
"finish"           return FINISH;
"break"            return BREAK;
"zero?"            return ZERO;
"notzero?"         return NOTZERO;
"if"               return IF;
"else"             return ELSE;
"print"            return PRINT;
"main"

[a-z]+[0-9_a-z]*     {
                          yylloc.last_line = yylloc.first_line = yylineno;
                          yylval.strVal = strdup(yytext);
                          return NAME;
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

[-\(\)<>=+\*/%;{},\[\]]  {
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


