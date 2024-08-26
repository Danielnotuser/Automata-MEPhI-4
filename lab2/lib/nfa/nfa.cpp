#include <iostream>
#include <format>
#include <sstream>
#include <stack>
#include <algorithm>
#include <random>

#include "nfa.h"

NFA NFA::all_nfa()
{
    NFA t(32);
    NFA uni = std::move(t);
    for (int i = 32; i < 127; i++)
    {
        NFA n(i);
        uni = union_nfa(uni, n);
    }
    return uni;
}

NFA NFA::plus_nfa(NFA& n1)
{
    NFA n;
    n1.end->epsilon.push_back(n1.start);
    n.start->epsilon.push_back(n1.start);
    n1.end->epsilon.push_back(n.end);
    return n;
}
NFA NFA::question_nfa(NFA& n1)
{
    NFA n;
    n = std::move(n1);
    n.start->epsilon.push_back(n.end);
    return n;
}
NFA NFA::repeat_nfa(Node* r, NFA& n1, const std::map<std::string, Node*> &gr)
{
    NFA res;
    auto b = static_cast<BraceNode*>(r);
    int fir = b->start, sec = b->end;
    NFA res_cat;
    if (fir) {
        res_cat = std::move(n1);
        for (int i = 1; i < fir; i++) {
            NFA temp = recur(r->left, gr);
            res_cat = concatenation_nfa(res_cat, temp);
        }
    }
    if (sec) // {?,!?}
    {
        if (fir == sec) res = std::move(res_cat); // {?,?}
        else
        {
            NFA temp = recur(r->left, gr);
            NFA res_uni = recur(r->left, gr);
            for (int j = 1; j < sec - fir; j++)
            {
                NFA t = recur(r->left, gr);
                temp = concatenation_nfa(temp, t);
                res_uni = union_nfa(res_uni, temp);
            }
            res_uni.start->epsilon.push_back(res_uni.end);
            if (fir) res = concatenation_nfa(res_cat, res_uni); // {?, !?}
            else res = std::move(res_uni); // {,!?}
        }
    }
    else if (!fir) // {,}
    {
        res.start->epsilon.push_back(n1.start);
        n1.end->epsilon.push_back(res.end);
        n1.end->epsilon.push_back(n1.start);
        res.start->epsilon.push_back(res.end);
    }
    else // {?,}
    {
        NFA temp = recur(r->left, gr);
        temp = question_nfa(temp);
        temp = plus_nfa(temp);
        res = concatenation_nfa(res_cat, temp);
    }

    return res;
}
NFA NFA::concatenation_nfa(NFA& n1, NFA& n2)
{
    NFA n;
    n1.end->epsilon = n2.start->epsilon;
    n1.end->edge = n2.start->edge;
    for (auto it = groups.begin(); it != groups.end(); it++)
    {
        if (it->second.first == n2.start) it->second.first = n1.end;
        if (it->second.second == n2.start) it->second.second = n1.end;
    }
    n.start = n1.start;
    n.end = n2.end;
    return n;

}
NFA NFA::union_nfa(NFA& n1, NFA& n2)
{
    NFA n;
    n.start->epsilon.push_back(n1.start);
    n.start->epsilon.push_back(n2.start);
    n1.end->epsilon.push_back(n.end);
    n2.end->epsilon.push_back(n.end);
    return n;
}

int NFA::in_group(Node *rt, const std::map<std::string, Node*> &gr, std::string &name)
{
    for (auto it = gr.begin(); it != gr.end(); it++)
        if (rt == it->second)
        {
            name = it->first;
            return 1;
        }
    return 0;
}

NFA NFA::recur(Node *rt, std::map<std::string, Node*> gr)
{
    std::string spec = "{.+-|?";
    NFA n1, n2;
    std::string name;
    if (rt->info == '&' && rt->left) {
        NFA a_nfa(rt->left->info);
        if (in_group(rt, gr, name)) groups[name] = std::make_pair(a_nfa.start, a_nfa.end);
        return NFA(rt->left->info);
    }
    if (rt->left) n1 = recur(rt->left, gr);
    if (rt->right) n2 = recur(rt->right, gr);
    if (spec.find(rt->info) == spec.npos) {
        NFA a_nfa(rt->info);
        if (in_group(rt, gr, name)) groups[name] = std::make_pair(a_nfa.start, a_nfa.end);
        return NFA(rt->info);
    }
    NFA n;
    switch (rt->info)
    {
        case '.':
            n = all_nfa(); break;
        case '{':
            n = repeat_nfa(rt, n1, gr); break;
        case '+':
            n = plus_nfa(n1); break;
        case '?':
            n = question_nfa(n1); break;
        case '-':
            n = concatenation_nfa(n1, n2); break;
        case '|':
            n = union_nfa(n1, n2); break;
    }
    if (in_group(rt, gr, name)) groups[name] = std::make_pair(n.start, n.end);
    return n;
}

NFA::NFA(STree &tr)
{
    alphabet = tr.alphabet;
    NFA n = recur(tr.root, tr.groups);
    start = n.start;
    end = n.end;
}

NFA::NFA(NFA&& n) noexcept
{
    start = n.start;
    end = n.end;
    alphabet = n.alphabet;
    n.start = nullptr;
    n.end = nullptr;
}

void rec_print(std::ofstream &f, StateNFA *v, int cnt)
{
    if (!v->edge.empty())
    {
        for (auto v1 = v->edge.begin(); v1 != v->edge.end(); v1++)
        {
            int new_cnt = rand();
            f << cnt << "->" << new_cnt << "[label=\"" << v1->first << "\"];\n";
            if (!v1->second->epsilon.empty() && v1->second->epsilon[0] == v) continue;
            rec_print(f, v1->second, new_cnt);
        }
    }

    if (!v->epsilon.empty())
    {
        for (size_t i = 0; i < v->epsilon.size(); i++)
        {
            int new_cnt = rand();
            f << cnt << "->" << new_cnt << "[label=\"eps\"];\n";
            rec_print(f, v->epsilon[i], new_cnt);
        }
    }

}

std::vector<StateNFA*> eps_closure(StateNFA* ch)
{
    std::vector<StateNFA*> res;
    res.push_back(ch);
    for (size_t i = 0; i < ch->epsilon.size(); i++)
    {
        std::vector<StateNFA*> add = eps_closure(ch->epsilon[i]);
        res.insert(res.end(), add.begin(), add.end());
    }
    return res;
}

StateNFA* transition(std::vector<StateNFA*> &ch, char c)
{
    for (size_t i = 0; i < ch.size(); i++)
        if (ch[i]->edge.find(c) != ch[i]->edge.end()) return ch[i]->edge[c];
    return nullptr;
}

int search(std::vector<StateNFA*> eps, StateNFA* ch)
{
    for (size_t i = 0; i < eps.size(); i++)
        if (eps[i] == ch) return 1;
    return 0;
}

int NFA::check_with_group(std::string pat, std::string name, std::string &res)
{
    if (groups.find(name) == groups.end()) { std::cout << "Error! No such group with this name." << std::endl; return 0; }
    StateNFA *ch = start;
    std::string res_gr;
    char last;
    std::vector<StateNFA*> eps = eps_closure(ch);
    bool read = false, check_end = false, check_start = false;
    for (auto c : pat)
    {
        if (search(eps, groups[name].first)) check_start = true;
        else if (check_start) {read = true; res_gr = last; check_start = false;}
        StateNFA *prev_ch = ch;
        ch = transition(eps, c);
        if (ch == nullptr) return 0;
        if (search(eps, groups[name].first) && ch != groups[name].first->edge[c]) read = false;
        else if (read) res_gr += c;
        if (check_start) last = c;
        eps = eps_closure(ch);
        if (search(eps, groups[name].second)) check_end = true;
        else if (check_end) {res_gr.erase(res_gr.begin() + res_gr.size() - 1); read = false; check_end = false;}

    }
    res = res_gr;
    return search(eps, end);
}

void NFA::print(const std::string& name)
{
    std::ofstream f;
    f.open(name);
    f << "digraph NFA {\nrankdir=\"LR\"\n";
    rec_print(f, start, 1);
    f << "}";
    f.close();
}