#include <iostream>
#include "lib/dfa/dfa.h"

int main()
{
    // "(<beb>n-p?-r-q)?-(<ded>y-t)?|a-h{,2}"
    // "(p|p+)|(p|p+)+"
	std::string pat = "a|b-c";
    std::cout << pat << std::endl;
    try {
        // Syntax tree synthesis
        STree tr;
        tr.synt(pat);
        tr.lcp(std::cout);
        // NFA synthesis
        NFA nf_auto(tr);
        nf_auto.print("../viz/nfa.dot");
        // DFA synthesis & checking
        std::string res;
        std::string gr_name = "quq";
        std::string ch = "cd";
        std::cout << "Checking \"" << ch << "\": " << std::endl;
        if (nf_auto.check_with_group(ch, gr_name, res))
            std::cout << "Captures group " << gr_name << " = " << res << std::endl;
        DFA df_auto(nf_auto);
        df_auto.print("../viz/dfa.dot");
        std::cout << "DFA: Checking string \"" << ch << "\" . . . ";
        if (df_auto.check(ch)) std::cout << "This string is ok!" << std::endl;
        else std::cout << "This string is NOT ok" << std::endl;
        // DFA minimization & checking
        df_auto.minimize();
        df_auto.print("../viz/min_dfa.dot");
        std::cout << "minDFA: Checking string \"" << ch << "\" . . . ";
        if (df_auto.check(ch)) std::cout << "This string is ok!" << std::endl;
        else std::cout << "This string is NOT ok" << std::endl;
        // K-path
        std::string k_p = df_auto.k_path();
        std::cout << "k-path: " << k_p;
        STree tr2;
        tr2.synt("a|b-c");
        NFA nf2(tr2);
        DFA df2(nf2);
        df2.minimize();
        df2.print("../viz/df2.dot");
        // DFA Intersection
        DFA sum_df = df_auto + df2;
        sum_df.print("../viz/sum_df.dot");
        // RegEx Inversion
        df_auto.inverse();
        df_auto.print("../viz/inv_df.dot");
    }
	catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
