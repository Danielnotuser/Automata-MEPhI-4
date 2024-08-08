#ifndef REGEX_DFA_H
#define REGEX_DFA_H

#include "../nfa/nfa.h"

class State{
public:
    std::map<char, State*> edge; // weight (alphabet) - State
    State() = default;
    State(const State& v) = default;
    ~State() = default;
};

class DFA {
private:
    State *start;
    std::vector<State*> end;
    std::vector<State*> not_end;
    std::vector<char> alphabet;
    std::map<int, State*> num;
    void eps_closure(StateNFA*, std::vector<StateNFA*>&);
    std::vector<StateNFA*> transition(std::vector<StateNFA*>&, char);
    bool same_group(std::vector<std::vector<State*>>&,State*, State*);
    std::map<int, State*> number(std::map<State*, int> &end_num);
    std::string inner_path(std::map<std::vector<int>, std::string> &expr, int i, int j, int k);
public:
    DFA() {start = new State;};
    explicit DFA(NFA&);
    DFA(DFA&&d) noexcept;
    void minimize();
    std::string k_path();
    int check(const std::string&);
    void print(const std::string&);
};

#endif //REGEX_DFA_H
