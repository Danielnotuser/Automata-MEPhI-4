#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include "dfa.h"

std::vector<StateNFA*> DFA::eps_closure(StateNFA* s)
{
    std::vector<StateNFA*> res;
    res.push_back(s);
    for (auto i : s->epsilon)
    {
        if (!i->epsilon.empty()) {
            std::vector<StateNFA *> temp = eps_closure(i);
            for (auto j : temp)
                res.push_back(j);
            temp.clear();
        }
        else
            res.push_back(i);

    }
    return res;
}

std::vector<StateNFA*> DFA::transition(std::vector<StateNFA*> &v, char c)
{
    std::vector<StateNFA*> res;
    for (auto vert : v)
        if (vert->edge[c]) res.push_back(vert->edge[c]);
    return res;
}

DFA::DFA(NFA& n)
{
    alphabet = n.get_alphabet();
    std::vector<State*> states; // DFA unprocessed states
    std::vector<std::vector<StateNFA*>> nfa_states;
    State *fir = new State;
    start = fir;
    std::vector<StateNFA*> temp = eps_closure(n.get_start());
    nfa_states.push_back(temp);
    states.push_back(fir);
    size_t i = 0;
    while (i < states.size())
    {
        State *check = states[i];
        temp = nfa_states[i];
        for (auto c : alphabet)
        {
            std::vector<StateNFA*> tr = transition(temp, c);
            std::vector<StateNFA*> temp_tr;
            for (auto v : tr)
            {
                auto temp_2 = eps_closure(v);
                for (auto t : temp_2)
                    temp_tr.push_back(t);
            }
            bool in_states = false;
            for (size_t j = 0; j < nfa_states.size(); j++) if ((in_states = nfa_states[j] == temp_tr)) {check->edge[c] = states[j]; break;}
            if (!in_states) {
                State *v = new State;
                check->edge[c] = v;
                nfa_states.push_back(temp_tr);
                states.push_back(v);
            }
        }
        i++;
    }
    StateNFA *last = n.get_end();
    for (size_t i = 0; i < nfa_states.size(); i++)
    {
        if (!nfa_states[i].empty() && std::find(nfa_states[i].begin(), nfa_states[i].end(), last) != nfa_states[i].end())
            end.push_back(states[i]);
    }
}

int DFA::check(const std::string &s)
{
    State *cur = start;
    for (auto c : s)
    {
        if (std::find(alphabet.begin(), alphabet.end(), c) == alphabet.end()) return 0;
        cur = cur->edge[c];
    }
    if (std::find(end.begin(), end.end(), cur) != end.end()) return 1;
    else return 0;
}