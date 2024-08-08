#include <iostream>
#include "lib/dfa/dfa.h"

int main()
{
    // "(<beb>n-p?-r-q)?-(<ded>y-t)?|a-h{,2}"
    // "(p|p+)|(p|p+)+"
	std::string pat = "a?+-p|p?";
    std::cout << pat << std::endl;
    STree tr;
    try {
        tr.synt(pat);
        tr.lcp(std::cout);
        NFA nf_auto(tr);
        //nf_auto.print("../viz/nfa.dot");
        DFA df_auto(nf_auto);
        df_auto.print("../viz/dfa.dot");
        std::string ch = "n";
        std::cout << "DFA: Checking string \"" << ch << "\" . . . ";
        if (df_auto.check(ch)) std::cout << "This string is ok!" << std::endl;
        else std::cout << "This string is NOT ok" << std::endl;
        df_auto.minimize();
        df_auto.print("../viz/min_dfa.dot");
        std::cout << "minDFA: Checking string \"" << ch << "\" . . . ";
        if (df_auto.check(ch)) std::cout << "This string is ok!" << std::endl;
        else std::cout << "This string is NOT ok" << std::endl;
        //std::cout << "DFA -> RegEx = " << df_auto.k_path();

    }
	catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
