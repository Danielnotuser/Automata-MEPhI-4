#ifndef REGEX_NFA_H
#define REGEX_NFA_H

#include "../tree/tree.h"
#include <fstream>

class StateNFA {
public:
    std::map<char, StateNFA*> edge; // weight (alphabet) - State from (!)
    std::vector<StateNFA*> epsilon;
    StateNFA() = default;
    StateNFA(const StateNFA& v) = default;
    StateNFA& operator=(const StateNFA& v) = default;
    ~StateNFA() = default;
};

class NFA {
private:
    StateNFA *start, *end;
    std::vector<char> alphabet;
    NFA all_nfa();
    NFA plus_nfa(NFA&);
    NFA question_nfa(NFA&);
    NFA repeat_nfa(Node*, NFA&);
    NFA concatenation_nfa(NFA&, NFA&);
    NFA union_nfa(NFA&, NFA&);
    NFA recur(Node *rt);
public:
    // constructors
    NFA() {start = new StateNFA; end = new StateNFA;};
    explicit NFA(char weight) {start = new StateNFA; end = new StateNFA; start->edge[weight] = end;};
    explicit NFA(STree&);
    NFA(NFA&& n) noexcept;
    // getters
    StateNFA *get_start() {return start;};
    StateNFA *get_end() {return end;};
    std::vector<char> get_alphabet() {return alphabet;};
    // overload
    NFA &operator=(NFA&& n) noexcept {start = n.start; end = n.end; n.start = nullptr; n.end = nullptr; return *this;};
    // other
    void print(const std::string&);

};

#endif //REGEX_NFA_H
