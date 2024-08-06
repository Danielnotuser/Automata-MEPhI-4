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
    void eps_closure(StateNFA*, std::vector<StateNFA*>&);
    std::vector<StateNFA*> transition(std::vector<StateNFA*>&, char);
    bool same_group(std::vector<std::vector<State*>>&,State*, State*);
public:
    DFA() : end() {start = new State;};
    explicit DFA(NFA&);
    DFA(DFA&&d) noexcept;
    void minimize();
    int check(const std::string&);
};

#endif //REGEX_DFA_H
