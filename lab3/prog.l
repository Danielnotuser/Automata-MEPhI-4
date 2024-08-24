%option case-insensitive
%option noyywrap c++
%option yylineno
%option yyclass="Reg_Scanner"
%{
#include <iostream>
#include <vector>
#include <string>
#include "type.h"
#include "scanner.h"

extern yy::location loc;

%}


%%

"const"            return yy::parser::make_CONST(loc);
"value"            return yy::parser::make_VALUE(loc);
"pointer"          return yy::parser::make_POINTER(loc);
"array of"         return yy::parser::make_ARRAYOF(loc);
"while"            return yy::parser::make_WHILE(loc);
"finish"           return yy::parser::make_FINISH(loc);
"break"            return yy::parser::make_BREAK(loc);
"return"           return yy::parser::make_RETURN(loc);
"zero?"            return yy::parser::make_ZERO(loc);
"notzero?"         return yy::parser::make_NOTZERO(loc);
"if"               return yy::parser::make_IF(loc);
"else"             return yy::parser::make_ELSE(loc);
"print"            return yy::parser::make_PRINT(loc);
"main"             {std::string s = strdup(yytext); return yy::parser::make_MAIN(s, loc);}

[a-z]+[0-9_a-z]*     {
                          loc.begin.line = loc.end.line = yylineno;
                          std::string n = strdup(yytext);
                          return yy::parser::make_NAME(n, loc);
                     }

0            {
                 loc.begin.line = loc.end.line = yylineno;
                 int i = atoi(yytext);
                 return yy::parser::make_INTEGER(i, loc);
             }

[1-9][0-9]*  {
                  loc.begin.line = loc.end.line = yylineno;
                  int i = atoi(yytext);
                  return yy::parser::make_INTEGER(i, loc);
              }

"+"                 return yy::parser::make_PLUS(loc);
"-"                 return yy::parser::make_MINUS(loc);
"*"                 return yy::parser::make_STAR(loc);
"/"                 return yy::parser::make_SLASH(loc);
"&"                 return yy::parser::make_ADDR(loc);
"%"                 return yy::parser::make_PERC(loc);
"("                 return yy::parser::make_LPAR(loc);
")"                 return yy::parser::make_RPAR(loc);
"="                 return yy::parser::make_ASSIGN(loc);
";"                 return yy::parser::make_SEMI(loc);
","                 return yy::parser::make_COMA(loc);
"{"                 return yy::parser::make_LBRACE(loc);
"}"                 return yy::parser::make_RBRACE(loc);
"["                 return yy::parser::make_LSQUARE(loc);
"]"                 return yy::parser::make_RSQUARE(loc);
"<"                 return yy::parser::make_LESS(loc);
">"                 return yy::parser::make_GREATER(loc);
">="                return yy::parser::make_GE(loc);
"<="                return yy::parser::make_LE(loc);
"!="                return yy::parser::make_NE(loc);
"=="                return yy::parser::make_EQ(loc);


\n               {/*yylloc.last_line = yylloc.first_line = yylineno;*/loc.step();}

[ \t]+           ;

.                {loc.begin.line = loc.end.line = yylineno; std::cerr << "Unknown character";}
%%


