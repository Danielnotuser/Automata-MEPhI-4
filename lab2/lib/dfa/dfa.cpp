#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include "dfa.h"

void DFA::eps_closure(StateNFA* s, std::vector<StateNFA*> &res)
{
    res.push_back(s);
    for (auto i : s->epsilon)
    {
        if (std::find(res.begin(), res.end(), i) != res.end()) continue;
        if (!i->epsilon.empty()) {
            eps_closure(i, res);
        }
        else
            res.push_back(i);

    }
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
    auto *fir = new State;
    start = fir;
    std::vector<StateNFA*> temp;
    eps_closure(n.get_start(), temp);
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
                std::vector<StateNFA*> temp_2;
                eps_closure(v, temp_2);
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
    for (size_t j = 0; j < nfa_states.size(); j++)
    {
        if (!nfa_states[j].empty() && std::find(nfa_states[j].begin(), nfa_states[j].end(), last) != nfa_states[j].end())
            end.push_back(states[j]);
        else
            not_end.push_back(states[j]);
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

DFA::DFA(DFA&&d) noexcept
{
    start = d.start;
    not_end = d.not_end;
    alphabet = d.alphabet;
    end = d.end;
    d.start = nullptr;
    d.not_end = std::vector<State*>();
    d.end = std::vector<State*>();
    d.alphabet = std::vector<char>();
}

bool DFA::same_group(std::vector<std::vector<State*>>& state_groups, State* s1, State *s2)
{
    for (auto c: alphabet)
    {
        for (auto s_v : state_groups)
        {
            for (auto s : s_v)
            {
                if ((s1->edge[c] == s && s2->edge[c] != s) ||
                        (s2->edge[c] == s && s1->edge[c] != s))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void del(std::vector<State*> &vect, std::vector<State*>&temp)
{
    for (auto t : temp)
    {
        for (size_t i = 0; i < vect.size(); i++)
        {
            if (vect[i] == t)
            {
                vect.erase(vect.begin() + i);
                break;
            }
        }
    }
}

bool has_end(std::vector<State*> &s1, std::vector<State*> s2)
{
    for (auto i : s2)
        for (auto j : s1)
            if (i == j) return true;
    return false;
}

void DFA::minimize()
{
    std::vector<std::vector<State*>> state_groups;
    state_groups.push_back(end);
    state_groups.push_back(not_end);
    for (size_t i = 0; i < state_groups.size(); i++)
    {
        if (state_groups[i].size() < 2) continue;
        int new_groups = 0;
        for (size_t j = 0; j < state_groups[i].size() - 1; j++)
        {
            std::vector<State*> temp;
            temp.push_back(state_groups[i][j]);
            for (size_t k = j + 1; k < state_groups[i].size(); k++)
            {
                if(same_group(state_groups, state_groups[i][j], state_groups[i][k]))
                {
                    temp.push_back(state_groups[i][k]);
                }
            }
            if (temp.size() > 1)
            {
                del(state_groups[i], temp);
                j = 0;
                new_groups++;
                state_groups.push_back(temp);
            }
        }
        if (state_groups[i].empty()) state_groups.erase(state_groups.begin() + i);
        if (!new_groups) break;
    }
    std::vector<State*> m_states(state_groups.size());
    std::vector<State*> new_end;
    for (size_t i = 0; i < state_groups.size(); i++)
    {
        if (std::find(state_groups[i].begin(), state_groups[i].end(), start) != state_groups[i].end())
            start = m_states[i];
        if (has_end(state_groups[i], end))
            new_end.push_back(m_states[i]);
        for (auto c : alphabet)
        {
            for (size_t j = 0; j < state_groups.size(); j++)
            {
                if (std::find(state_groups[j].begin(), state_groups[j].end(), state_groups[i][0]->edge[c]) != state_groups[j].end())
                {
                    m_states[i]->edge[c] = m_states[j];
                    break;
                }
            }
        }
    }
}