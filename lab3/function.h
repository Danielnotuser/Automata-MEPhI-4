#ifndef REGEX_TABLE_H
#define REGEX_TABLE_H

#include <vector>
#include <algorithm>
#include <cstdarg>
#include <map>
#include "type.h"

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

        std::string get_name() {return name;};
        VariableTable get_tab() {return var_tab;};
        void set_ptr(Node *ptr) {n_ptr = ptr;};
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





#endif //REGEX_TABLE_H
