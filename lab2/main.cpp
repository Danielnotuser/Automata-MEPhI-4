#include <iostream>
#include "lib/dfa/dfa.h"

int main()
{
    // "(<beb>n-p?-r-q)?-(<ded>y-t)?|a-h{,2}"
	std::string pat = ".-p+-m";
    std::cout << pat << std::endl;
    STree tr;
    try {
        tr.synt(pat);
        tr.lcp(std::cout);
        NFA nf_auto(tr);
        std::ofstream f;
        f.open("../viz/nfa.dot");
        nf_auto.print(f);
        f.close();
        DFA df_auto(nf_auto);
        std::string ch = "pm";
        std::cout << "Checking string \"" << ch << "\" . . . ";
        if (df_auto.check(ch)) std::cout << "This string is ok!" << std::endl;
        else std::cout << "This string is NOT ok" << std::endl;
    }
	catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
