#ifndef REGEX_TREE_H
#define REGEX_TREE_H

#include <string>
#include <map>


class Node {
    public:
        char info;
        Node *left;
        Node *right;
        Node() : info(), left(nullptr), right(nullptr) {};
        explicit Node(char info) : info(info), left(nullptr), right(nullptr) {};
        ~Node() = default;
};

class BraceNode : public Node {
    public:
        int start, end;
        BraceNode() : Node(), start(-1), end(-1) {};
        explicit BraceNode(char info) : Node(info), start(-1), end(-1) {};
        BraceNode(int start, int end) : Node(), start(start), end(end) {};
        ~BraceNode() = default;
};

class STree {
    private:
        Node *root;
        std::map<std::string, Node*> groups;
    public:
        // constructors
        STree() : root(nullptr) {};
        STree(Node *rt) : root(rt) {};
        // getters
        Node *get_root() {return root;};
        std::map<std::string, Node*> get_groups() {return groups;};
        // synthesis & detour
        void synt(std::string&);
        void lcp();

        ~STree();
};

#endif //REGEX_TREE_H
