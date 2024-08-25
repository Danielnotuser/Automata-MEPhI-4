#ifndef REGEX_TABLE_H
#define REGEX_TABLE_H

#include <vector>
#include <algorithm>
#include <cstdarg>
#include <map>
#include "type.h"


class VariableTable {
    private:
        std::map<std::string, Variable> vars;
    public:
        VariableTable() : vars() {};
        VariableTable(std::map<std::string, Variable> m) : vars(m) {};
        int find_var(std::string var_name, Variable &res);
        void insert(Variable &p) {vars.insert(std::make_pair(p.get_name(), p));};
        void insert(VariableTable &a) {vars.insert(a.vars.begin(), a.vars.end());};
        void clear() {vars.clear();};
        bool empty() {return vars.empty();};
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
        Function(int ret_type, char *name, VariableTable args) : ret_type(ret_type), name(name), args(args) {};
        Function(int ret_type, char *name, VariableTable args, Node *n_ptr) : ret_type(ret_type), name(name), args(args), n_ptr(n_ptr) {};

        int find_var(std::string var_name, Variable &res) {return var_tab.find_var(var_name, res);};
        void insert_var(Variable &v) {return var_tab.insert(v);};

        std::string get_name() {return name;};
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

        int find_func(std::string func_name, Function& f);
        int find_var(std::string func_name, std::string var_name, Variable &res);

        void insert_var(std::string fname, Variable &v) { func[fname].insert_var(v); };
        void insert_func(Function f) { func[f.get_name()] = f; };
};





#endif //REGEX_TABLE_H
