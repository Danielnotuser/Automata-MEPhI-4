#ifndef TYPE_H
#define TYPE_H

#include <any>
#include <string>

class Node {
    public:
        int type;

        Node() = default;
        virtual std::any ex() = 0;
        int get_type() { return type; };
        ~Node() = default;
};

class Operation : public Node {
    private:
        int id;
        int nops;
        Node **op;
    public:
        Operation(int id, int nops, ...);
        std::any ex() override;
        ~Operation() = default;

};

class Number : public Node {
    private:
        int num;
    public:
        Number() = default;
        Number(int num) : num(num) { type = -2; };
        std::any ex() override { return num; };
        ~Number() = default;
};

class Variable : public Node {
    private:
        int size;
        int exp;
        std::string name;
        std::any var;
    public:
        Variable() : size(0) {};
        Variable(int var_type, std::string name, std::any value);
        explicit Variable(const Variable&);

        std::any ex() override { return var; };
        bool empty() { return !var.has_value(); }

        Variable &set_var(std::any);

        ~Variable() = default;

};


std::any execute(std::string, Node *);

#endif