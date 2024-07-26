#include <iostream>
#include <string>
#include "tree.h"

int main()
{
	std::string pat = "m|(n-p+-r|q)+-e";
    std::cout << pat << std::endl;
    STree tr;
    try {
        tr.synt(pat);
    }
	catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    tr.lcp();
    return 0;
}
