#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <queue>

#include "dfa.h"

void DFA::eps_closure(StateNFA* s, std::vector<StateNFA*> &res)
{
    res.push_back(s);
    for (size_t i = 0; i < s->epsilon.size(); i++)
    {
        if (std::find(res.begin(), res.end(), s->epsilon[i]) != res.end()) continue;
        if (!s->epsilon[i]->epsilon.empty()) {
            eps_closure(s->epsilon[i], res);
        }
        else
            res.push_back(s->epsilon[i]);

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

DFA &DFA::operator=(DFA&&d) noexcept
{
    start = d.start;
    not_end = d.not_end;
    alphabet = d.alphabet;
    end = d.end;
    d.start = nullptr;
    d.not_end = std::vector<State*>();
    d.end = std::vector<State*>();
    d.alphabet = std::vector<char>();
    return *this;
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
    if (!end.empty()) state_groups.push_back(end);
    if (!not_end.empty()) state_groups.push_back(not_end);
    size_t s = 0;
    while (s < state_groups.size())
    {
        if (state_groups[s].size() < 2) {s++; continue;}
        int new_groups = 0;
        size_t j = 0;
        while (j < state_groups[s].size())
        {
            std::vector<State*> temp;
            temp.push_back(state_groups[s][j]);
            for (size_t k = j + 1; k < state_groups[s].size(); k++)
            {
                if(same_group(state_groups, state_groups[s][j], state_groups[s][k]))
                {
                    temp.push_back(state_groups[s][k]);
                }
            }
            if (temp.size() != state_groups[s].size())
            {
                del(state_groups[s], temp);
                j = 0;
                new_groups++;
                state_groups.push_back(temp);
            }
            else
            {
                new_groups++;
                break;
            }

        }
        if (!new_groups) break;
        s++;
    }
    std::vector<State*> m_states;
    for (size_t i = 0; i < state_groups.size(); i++) {auto *new_s = new State; m_states.push_back(new_s);}
    std::vector<State*> new_end, new_not_end;
    for (size_t i = 0; i < state_groups.size(); i++)
    {
        if (std::find(state_groups[i].begin(), state_groups[i].end(), start) != state_groups[i].end())
            start = m_states[i];
        if (has_end(state_groups[i], end))
            new_end.push_back(m_states[i]);
        else
            new_not_end.push_back(m_states[i]);
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
    end = new_end;
    not_end = new_not_end;
}

std::map<int, State*> DFA::number(std::map<State*, int> &end_num)
{
    std::map<int, State*> res;
    res[1] = start;
    if (std::find(end.begin(), end.end(), start) != end.end()) end_num[start] = 1;
    std::vector<State*> seen;
    std::queue<State*> q;
    q.push(start);
    seen.push_back(start);
    int cnt = 2;
    while (!q.empty())
    {
        State *s = q.front();
        q.pop();
        for (auto s1 : s->edge)
        {
            if (std::find(seen.begin(), seen.end(), s1.second) == seen.end())
            {
                if (std::find(end.begin(), end.end(), s1.second) != end.end()) end_num[s1.second] = cnt;
                res[cnt] = s1.second;
                seen.push_back(s1.second);
                q.push(s1.second);
                cnt++;
            }
        }
    }
    return res;
}

std::string DFA::inner_path(std::map<std::vector<int>, std::string> &expr, int i, int j, int k)
{
    std::vector<int> arg = {i, j, k};
    if (auto e = expr.find(arg); e != expr.end())
        return e->second;
    std::string res = "";
    if (k == 0)
    {
        std::string spec = "<>{,}()+-|?&";
        for (auto s = num[i]->edge.begin(); s != num[i]->edge.end(); s++)
        {
            if (s->second == num[j])
            {
                char s1 = s->first;
                if (res.empty()) {if (spec.find(s1) != spec.npos) res = "&"; res += s1;}
                else {res += "|"; if (spec.find(s1) != spec.npos) res = "&"; res += s1;}
            }
        }
        expr[arg] = res;
        return res;
    }
    // Rij(k) = Rij(k-1) | Rik(k-1) - (Rkk(k-1))?+ - Rjk(k-1)
    std::string s1 = inner_path(expr, i, j, k - 1); // Rij (k-1)
    std::string s2 = inner_path(expr, i, k, k - 1); // Rik (k-1)
    std::string s3 = inner_path(expr, k, k, k - 1); // Rkk (k-1)
    std::string s4 = inner_path(expr, k, j, k - 1); // Rkj (k-1)
    if (i == k && j == k)
    {
        if (!s1.empty()) res = "(" + s1 + ")+";
        expr[arg] = res;
        return res;
    }
    else if (j == k)
    {
        if (!s1.empty()) res = s1;
        if (!s3.empty()) res += "-(" + s3 + ")?+";
        expr[arg] = res;
        return res;
    }
    std::string sec_part = "";
    if (!s1.empty())
        res = "(" + s1 + ")";
    if (!s2.empty())
    {
        sec_part += "(" + s2 + ")";
    }
    if (!s3.empty())
    {
        if (!s2.empty()) sec_part += "-(" + s3 + ")?+";
        else sec_part += "(" + s3 + ")?+";
    }
    if (!s4.empty())
    {
        if (!s3.empty() || !s2.empty()) sec_part += "-(" + s4 + ")";
        else sec_part += "(" + s4 + ")";
    }
    if (res != sec_part && !sec_part.empty() && !res.empty()) res = res + "|" + sec_part;
    else if (res.empty()) res = sec_part;

    expr[arg] = res;
    return res;
}

std::string DFA::k_path()
{
    std::map<State*, int> end_num;
    num = number(end_num);

    std::string res;
    std::map<std::vector<int>, std::string> expr;
    for (size_t i = 0; i < end.size(); i++)
    {
        std::string temp = inner_path(expr, 1, end_num[end[i]], num.size());
        if (i == 0) res = temp;
        else
        {
            if (res.empty()) res = temp;
            else if (temp.empty()) res = "(" + res + ")?";
            else res += "|" + temp;
        }
    }
    /*for (auto s: expr)
    {
        for (auto i : s.first)
            std::cout << i << " ";
        std::cout << "= " << s.second << std::endl;
    }*/
    return res;
}

void dfa_print(std::ofstream &f, std::vector<State*> &st, State *v, std::map<int, State*> &num)
{
    st.push_back(v);
    int cnt;
    for (auto n : num)
        if (n.second == v) cnt = n.first;
    for (auto v1 = v->edge.begin(); v1 != v->edge.end(); v1++)
    {
        int new_cnt;
        for (auto n: num)
            if (n.second == v1->second) new_cnt = n.first;
        f << cnt << "->" << new_cnt << "[label=\"" << v1->first << "\"];\n";
        if (std::find(st.begin(), st.end(), v1->second) == st.end()) dfa_print(f, st, v1->second, num);
    }
}

void DFA::print(const std::string &name)
{
    std::map<State*, int> t;
    num = number(t);
    std::ofstream f;
    f.open(name);
    f << "digraph DFA {\nrankdir=\"LR\"\n";
    std::vector<State*> st;
    dfa_print(f, st, start, num);
    f << "}";
    f.close();
}

DFA DFA::operator+(DFA& d)
{
    std::vector<State*> all_states;
    all_states.insert(all_states.end(), not_end.begin(), not_end.end());
    all_states.insert(all_states.end(), end.begin(), end.end());
    std::vector<State*> d_states;
    d_states.insert(d_states.end(), d.not_end.begin(), d.not_end.end());
    d_states.insert(d_states.end(), d.end.begin(), d.end.end());
    DFA res;
    res.alphabet = alphabet;
    std::vector<State*> res_states;
    std::vector<std::pair<State*, State*>> res_depend;
    for (auto s1 : all_states)
    {
        for (auto s2 : d_states)
        {
            auto *s = new State;
            res_depend.emplace_back(s1, s2);
            res_states.push_back(s);
            if (std::find(end.begin(), end.end(), s1) != end.end() && std::find(d.end.begin(), d.end.end(), s2) != d.end.end())
                res.end.push_back(s);
            else res.not_end.push_back(s);
            if (s1 == start && s2 == d.start) res.start = s;
        }
    }
    for (size_t i = 0; i < res_states.size(); i++)
    {
        for (auto s : res_depend[i].first->edge)
        {
            if (auto f = res_depend[i].second->edge.find(s.first); f != res_depend[i].second->edge.end())
            {
                int cnt = -1;
                for (size_t j = 0; j < res_depend.size(); j++)
                    if (res_depend[j] == std::make_pair(s.second, f->second)) {cnt = j; break;}
                if (cnt != -1)
                    res_states[i]->edge[s.first] = res_states[cnt];
            }
        }
    }
    return res;
}

void DFA::inverse()
{
    std::vector<State*> all_states, new_start;
    State* new_end;
    all_states.insert(all_states.end(), not_end.begin(), not_end.end());
    all_states.insert(all_states.end(), end.begin(), end.end());
    std::vector<std::pair<State*, State*>> edge_map;
    std::vector<char> char_map;
    for (size_t i = 0; i < all_states.size(); i++)
    {
        if (std::find(end.begin(), end.end(), all_states[i]) != end.end()) new_start.push_back(all_states[i]);
        if (all_states[i] == start) new_end = all_states[i];
        for (size_t j = 0; j < all_states.size(); j++)
        {
            if (i == j) continue;
            for (auto c: alphabet)
            {
                if (all_states[i]->edge[c] == all_states[j])
                {
                    edge_map.emplace_back(all_states[i], all_states[j]);
                    char_map.push_back(c);
                }
            }
        }
        all_states[i]->edge.clear();
    }

    for (size_t i = 0; i < edge_map.size(); i++)
    {
        edge_map[i].second->edge[char_map[i]] = edge_map[i].first;
    }

    std::vector<StateNFA*> nfa_states(all_states.size()), nfa_start;
    for (size_t i = 0; i < nfa_states.size(); i++) nfa_states[i] = new StateNFA;
    auto* nfa_end = new StateNFA;
    for (size_t i = 0; i < all_states.size(); i++)
    {
        if (std::find(new_start.begin(), new_start.end(), all_states[i]) != new_start.end()) nfa_start.push_back(nfa_states[i]);
        if (all_states[i] == new_end) nfa_end = nfa_states[i];
        for (auto it = all_states[i]->edge.begin(); it != all_states[i]->edge.end(); it++)
        {
            for (size_t j = 0; j < all_states.size(); j++)
            {
                if (it->second == all_states[j]) {
                    nfa_states[i]->edge.insert(
                            {it->first,
                             nfa_states[j]});
                    break;
                }
            }
        }
    }

    StateNFA *act_start = new StateNFA;
    for (size_t i = 0; i < nfa_start.size(); i++)
    {
        act_start->epsilon.push_back(nfa_start[i]);
    }
    NFA n(act_start, nfa_end, alphabet);
    n.print("../viz/inv_nfa.dot");
    DFA inv(n);
    inv.print("../viz/inv.dot");
    *this = std::move(inv);
}