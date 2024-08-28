#ifndef TYPE_H
#define TYPE_H

#include <any>
#include <string>
#include <map>

class Node {
    public:
        int type;

        Node() = default;
        Node(int type) : type(type) {};
        virtual std::any ex() = 0;
        int get_type() { return type; };
        ~Node() = default;
};

class Number : public Node {
    private:
        int num;
    public:
        Number() = default;
        Number(int num) : Node(-2), num(num) {};
        std::any ex() override { return num; };
        ~Number() = default;
};

class Variable : public Node {
private:
    int size;
    int exp;
    std::string name;
    std::any val;
public:
    Variable() : size(0) {};
    Variable(int var_type, std::string name) : name(name) {type = var_type;};
    void init(std::any a = std::any());
    explicit Variable(const Variable&);

    std::any ex() override { return val; };
    bool empty() { return !val.has_value(); }

    std::string get_name() {return name;};
    std::any get_val() {return val;}
    Variable &set_var(std::any);

    Variable& operator=(const Variable &);
    ~Variable() = default;

};

class VariableTable {
private:
    std::map<std::string, Variable> vars;
public:
    VariableTable() : vars() {};
    VariableTable(std::map<std::string, Variable> m) : vars(m) {};
    VariableTable(const VariableTable& v) {vars = v.vars;};
    int find_var(std::string var_name, Variable *res);
    void insert(Variable &p) {vars.insert(std::make_pair(p.get_name(), p));};
    void insert(VariableTable &a) {vars.insert(a.vars.begin(), a.vars.end());};
    int size() {return vars.size();};
    void clear() {vars.clear();};
    bool empty() {return vars.empty();};
};

class Operation : public Node {
    private:
        int id;
        int nops;
        Node **op;
        VariableTable *func_vars;
        VariableTable *glob_vars;
        std::any arr_el_ref(Node *var_node, Node *ind_node);
        Variable *ref_name(Node*);
    public:
        Operation(VariableTable *glob, VariableTable *func, int id, int nops, ...);
        Operation(VariableTable *func, int id, int nops, ...);
        Operation(int id, int nops, ...);
        std::any ex() override;
        ~Operation() = default;

};

std::any execute(std::string, Node *);

#endif