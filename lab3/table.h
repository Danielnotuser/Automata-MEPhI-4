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
        VariableTable() = default;
        VariableTable(std::map<std::string, Variable> m) : vars(m) {};
        int find_var(std::string name, Variable &res)
        {
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
            for (auto v = vars.begin(); v != vars.end(); v++)
                if (v->first == name)
                {
                    res = v->second;
                    return 1;
                }
            return 0;
        };
};

class Function {
    public:
        int ret_type;
        std::string name;
        VariableTable var_tab;
        std::map<std::string, Variable> args;
        Node *tree;

        Function() = default;
        Function(int ret_type, std::string name, std::map<std::string, Variable> args) : ret_type(ret_type), name(name), args(args) {};
        int find_var(std::string var_name, Variable &res) {return var_tab.find(var_name, res);};
};

class FunctionTable {
    private:
        std::map<std::string, Function> func;
    public:
        FunctionTable() = default;
        FunctionTable(std::map<std::string, Function> f) : func(f) {};

        int find_func(std::string name, Function &res)
        {
            if (name.empty()) return 0;
            std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
            for (auto v = func.begin(); v != func.end(); v++)
                if (v->first == name)
                {
                    res = v->second;
                    return 1;
                }
            return 0;
        };

        int find_var(std::string var_name, std::string func_name, Variable &res)
        {
            if (func_name.empty() || var_name.empty()) return 0;
            Function f;
            if (!find_func(func_name, f)) return 0;
            return f.find_var(var_name, res);
        }
};




#endif //REGEX_TABLE_H
