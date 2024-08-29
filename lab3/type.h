#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <any>
#include <string>
#include <map>
#include <vector>
#include <cstdarg>
#include <algorithm>
#include "robot.h"

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
    void pointer_init(Variable*);
    explicit Variable(const Variable&);

    std::any ex() override { return val; };
    bool empty() { return !val.has_value(); }

    std::string get_name() {return name;};
    std::any &get_val() {return val;};
    int* get_addr();
    const int* get_const_addr();
    int get_size() {return size;};
    Variable &set_var(std::any);

    Variable& operator=(const Variable &);
    std::any arr_ref(int ind, std::any expr = std::any());
    std::any pointer_deref(std::any expr = std::any());
    ~Variable() = default;

};

class VariableTable {
private:
    std::vector<std::pair<std::string, Variable>> vars;
public:
    VariableTable() = default;
    VariableTable(std::vector<std::pair<std::string, Variable>> m) : vars(m) {};
    VariableTable(const VariableTable& v) {vars = v.vars;};

    int find_var(std::string var_name, Variable *res);
    void insert(Variable &p) {vars.push_back(std::make_pair(p.get_name(), p));};
    void insert(VariableTable &a) {vars.insert(vars.end(), a.vars.begin(), a.vars.end());};

    void reset_var(std::string name, std::any val);
    void reset_all(std::vector<std::any>);
    int size() {return vars.size();};
    void clear() {vars.clear();};
    bool empty() {return vars.empty();};
};

class NodeArr {
public:
    std::vector<Node*> nodes;
    NodeArr() : nodes() {};
    NodeArr(std::vector<Node*> n) : nodes(n) {};

};

class Function {
private:
    int ret_type;
    std::string name;
    VariableTable var_tab;
    VariableTable args;
    Node *n_ptr;
public:
    Function() : var_tab(), args(), n_ptr(nullptr) {};
    Function(int ret_type, char *name, VariableTable args) : ret_type(ret_type), name(name), args(args) {var_tab.insert(args);};
    Function(int ret_type, char *name, VariableTable args, Node *n_ptr) : ret_type(ret_type), name(name), args(args), n_ptr(n_ptr) {var_tab.insert(args);};

    int find_var(std::string var_name, Variable *res) {return var_tab.find_var(var_name, res);};
    void insert_var(Variable &v) {var_tab.insert(v);};

    int get_ret_type() {return ret_type;};
    std::string get_name() {return name;};
    VariableTable get_tab() {return var_tab;};
    void set_ptr(Node *ptr) {n_ptr = ptr;};

    void update_args(std::vector<std::any> v) {args.reset_all(v);}
    std::any execute();
};

class FunctionTable {
private:
    std::map<std::string, Function> func;
public:
    FunctionTable() = default;
    FunctionTable(std::map<std::string, Function> f) : func(f) {};

    Function get_func(std::string name) { return func[name]; };
    VariableTable get_tab(std::string name) { return func[name].get_tab(); }

    int find_func(std::string func_name, Function* f);
    int find_var(std::string func_name, std::string var_name, Variable *res);

    void insert_var(std::string fname, Variable &v) { func[fname].insert_var(v); };
    void insert_func(Function f) { func[f.get_name()] = f; };


};

class Operation : public Node {
    private:
        int id;
        int nops;
        Node **op;
        VariableTable *func_vars;
        VariableTable *glob_vars;
        Function *f;
        Robot *rob;
        void reset_var(Node *p, std::any val);
        std::any pointer_deref(Node *p, Node *expr = nullptr);
        std::any arr_elref(Node *arr, Node *ind, Node *expr = nullptr);
        Variable *ref_name(Node*);
        std::any address(Node *p);
        void call_func();
    public:
        Operation() : id(0), op(nullptr) {type = -1;};
        Operation(Robot *r, int id) : id(id), rob(r) {};                                                // Robot operation
        Operation(Function *f, VariableTable *glob, VariableTable *func, int id, NodeArr n);            // Call Function operation
        Operation(VariableTable *glob, VariableTable *func, int id, int nops, ...);                     // Value init/ref operation
        Operation(VariableTable *func, int id, int nops, ...);                                          // Value glob_init operation
        Operation(int id, int nops, ...);                                                               // Default operation
        std::any ex() override;
        int get_id() { return id; };
        ~Operation() = default;

};



std::any execute(std::string, Node *);

#endif