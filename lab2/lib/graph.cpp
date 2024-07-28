#include <iostream>
#include <vector>
#include <format>
#include <sstream>

#include "graph.h"

NFA all_nfa();
NFA plus_nfa(NFA&);
NFA question_nfa(NFA&);
NFA repeat_nfa(NFA&);
NFA concatenation_nfa(NFA&, NFA&);
NFA union_nfa(NFA&, NFA&);

NFA recur(Node *rt, const std::string &spec)
{
    NFA n1, n2;
    if (rt->left) n1 = recur(rt->left, spec);
    if (rt->right) n2 = recur(rt->right, spec);
    if (spec.find(rt->info) == spec.npos)
        return NFA(rt->info);
    switch (rt->info)
    {
        case '.':
            return all_nfa();
        case '{':
            return repeat_nfa(n1);
        case '+':
            return plus_nfa(n1);
        case '?':
            return question_nfa(n1);
        case '-':
            return concatenation_nfa(n1, n2);
        case '|':
            return union_nfa(n1, n2);
    }
}

NFA::NFA(STree &tr)
{
    std::string spec = "<>{}(),.+-|";
    recur(tr.get_root(), spec);
}