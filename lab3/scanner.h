#ifndef REGEX_SCANNER_H
#define REGEX_SCANNER_H


#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "y.tab.h"

#undef YY_DECL
#define YY_DECL yy::parser::symbol_type Reg_Scanner::s_yylex()
#undef yyterminate
#define yyterminate() return yy::parser::symbol_type()

const std::string inp_file = "code.txt";
yy::position pos_file(&inp_file);
yy::location loc(pos_file);

class Reg_Scanner : public yyFlexLexer {
public:
    yy::parser::symbol_type s_yylex();
};



#endif //REGEX_SCANNER_H
