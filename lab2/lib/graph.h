#ifndef REGEX_GRAPH_H
#define REGEX_GRAPH_H

#include "tree.h"

typedef struct Vertex {
    std::map<char, Vertex*> edge; // weight (alphabet) - vertex
} Vertex;

class NFA {
    private:
        Vertex *start;
        Vertex *end;
    public:
        NFA() : start(nullptr), end(nullptr) {};
        explicit NFA(char weight) {start = new Vertex; end = new Vertex; start->edge[weight];};
        NFA(Vertex *start, Vertex *end) : start(start), end(end) {};
        NFA(STree&);
};
#endif //REGEX_GRAPH_H
