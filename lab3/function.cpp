#include <iostream>
#include "table.h"

int VariableTable::find_var(std::string name, Variable *res)
{
    if (name.empty()) return 0;
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
    for (auto v = vars.begin(); v != vars.end(); v++)
        if (v->first == name)
        {
            *res = v->second;
            return 1;
        }
    return 0;
};

std::any Function::execute()
{
    var_tab.insert(args);
    if (n_ptr->type == -2) { Number *n = static_cast<Number*>(n_ptr); return n->ex(); }
    else if (n_ptr->type == -1) { Operation *op = static_cast<Operation*>(n_ptr); return op->ex(); }
    else { Variable *v = static_cast<Variable*>(n_ptr); return v->ex(); }
}


int FunctionTable::find_func(std::string name, Function *res)
{
    if (name.empty()) return 0;
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
    for (auto v = func.begin(); v != func.end(); v++)
        if (v->first == name)
        {
            *res = v->second;
            return 1;
        }
    return 0;
}

int FunctionTable::find_var(std::string func_name, std::string var_name, Variable *res)
{
    if (func_name.empty() || var_name.empty()) return 0;
    Function f;
    if (!find_func(func_name, &f)) return 0;
    return f.find_var(var_name, res);
}
