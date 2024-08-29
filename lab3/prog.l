%option case-insensitive
%option noyywrap
%option yylineno

%{
#include "type.h"

struct val  {
	char* strVal;
	int intVal;
	Node* nPtr;
};

#define YYSTYPE val

#include <stdlib.h>
#include "y.tab.h"


void yyerror(const char*);

%}


%%

"const"            return CONST;
"value"            return VALUE;
"pointer"          return POINTER;
"array of"         return ARRAYOF;
"while"            return WHILE;
"finish"           return FINISH;
"break"            return BREAK;
"return"           return RETURN;
"zero?"            return ZERO;
"notzero?"         return NOTZERO;
"if"               return IF;
"else"             return ELSE;
"print"            return PRINT;
"size"             return SIZE;
"top"              return TOP;
"bottom"           return BOTTOM;
"left"             return LEFT;
"right"            return RIGHT;
"portal"           return PORTAL;
"teleport"         return TELEPORT;
"main"             {yylval.strVal = strdup(yytext); return MAIN;}

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

"+"                 return PLUS;
"-"                 return MINUS;
"*"                 return STAR;
"/"                 return SLASH;
"&"                 return ADDR;
"%"                 return PERC;
"("                 return LPAR;
")"                 return RPAR;
"="                 return ASSIGN;
";"                 return SEMI;
","                 return COMA;
"{"                 return LBRACE;
"}"                 return RBRACE;
"["                 return LSQUARE;
"]"                 return RSQUARE;
"<"                 return LESS;
">"                 return GREATER;
">="                return GE;
"<="                return LE;
"!="                return NE;
"=="                return EQ;


\n               {yylloc.last_line = yylloc.first_line = yylineno;}

[ \t]+           ;

.                {yylloc.last_line = yylloc.first_line = yylineno; yyerror("Unknown character");}
%%


