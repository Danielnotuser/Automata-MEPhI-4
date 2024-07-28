#include <iostream>
#include "lib/tree.h"
#include "lib/graph.h"

int main()
{
	std::string pat = "m{1,2}|(<beb>n-p+-r|q)+-(<ded>y-t)+";
    std::cout << pat << std::endl;
    STree tr;
    try {
        tr.synt(pat);
        NFA nf_auto(tr);
    }
	catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    tr.lcp();
    return 0;
}
