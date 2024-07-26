#ifndef TREE_H
#define TREE_H

#include <string>

class Node {
    public:
        char info;
        Node *left;
        Node *right;
        Node() : left(nullptr), right(nullptr), info() {};
        explicit Node(char info) : info(info), left(nullptr), right(nullptr) {};
        ~Node() = default;
};

class STree {
    private:
        Node *root;
    public:
        STree() : root(nullptr) {};
        void synt(std::string&);
        void lcp();
        ~STree();
};

#endif
