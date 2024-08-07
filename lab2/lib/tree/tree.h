#ifndef REGEX_TREE_H
#define REGEX_TREE_H

#include <string>
#include <map>
#include <vector>


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
        BraceNode() : Node(), start(0), end(0) {};
        explicit BraceNode(char info) : Node(info), start(0), end(0) {};
        BraceNode(int start, int end) : Node(), start(start), end(end) {};
        ~BraceNode() = default;
};

class STree {
    public:
        Node *root;
        std::map<std::string, Node*> groups;
        std::vector<char> alphabet;
        // constructors
        STree() : root(nullptr) {};
        STree(Node *rt) : root(rt) {};
        // synthesis & detour
        void synt(const std::string&);
        void lcp(std::ostream &c);

        ~STree();
};

#endif //REGEX_TREE_H
