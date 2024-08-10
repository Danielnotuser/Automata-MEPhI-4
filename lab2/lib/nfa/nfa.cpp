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
NFA NFA::repeat_nfa(Node* r, NFA& n1)
{
    NFA res;
    auto b = static_cast<BraceNode*>(r);
    int fir = b->start, sec = b->end;
    NFA res_cat;
    if (fir) {
        res_cat = std::move(n1);
        for (int i = 1; i < fir; i++) {
            NFA temp = recur(r->left);
            res_cat = concatenation_nfa(res_cat, temp);
        }
    }
    if (sec) // {?,!?}
    {
        if (fir == sec) res = std::move(res_cat); // {?,?}
        else
        {
            NFA temp = recur(r->left);
            NFA res_uni = recur(r->left);
            for (int j = 1; j < sec - fir; j++)
            {
                NFA t = recur(r->left);
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
        NFA temp = recur(r->left);
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

NFA NFA::recur(Node *rt)
{
    std::string spec = "{.+-|?";
    NFA n1, n2, n;
    if (rt->info == '&' && rt->left) return NFA(rt->left->info);
    if (rt->left) n1 = recur(rt->left);
    if (rt->right) n2 = recur(rt->right);
    if (spec.find(rt->info) == spec.npos)
    {n = NFA(rt->info); return n;}
    switch (rt->info)
    {
        case '.':
            return all_nfa();
        case '{':
            return repeat_nfa(rt, n1);
        case '+':
            return plus_nfa(n1);
        case '?':
            return question_nfa(n1);
        case '-':
            return concatenation_nfa(n1, n2);
        case '|':
            return union_nfa(n1, n2);
    }
    return n;
}

NFA::NFA(STree &tr)
{
    alphabet = tr.alphabet;
    NFA n = recur(tr.root);
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

void NFA::print(const std::string& name)
{
    std::ofstream f;
    f.open(name);
    f << "digraph NFA {\nrankdir=\"LR\"\n";
    rec_print(f, start, 1);
    f << "}";
    f.close();
}