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
    std::vector<StateNFA*> eps_closure(StateNFA*);
    std::vector<StateNFA*> transition(std::vector<StateNFA*> &v, char c);
    std::vector<char> alphabet;
public:
    DFA() : end() {start = new State;};
    explicit DFA(NFA&);
    int check(const std::string&);
};

#endif //REGEX_DFA_H
