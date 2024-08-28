#include <iostream>
#include "function.h"

int VariableTable::find_var(std::string name, Variable *res)
{
    if (name.empty()) return 0;
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
    if (vars.find(name) != vars.end())
    {
        *res = vars[name];
        return 1;
    }
    else return 0;
};

void VariableTable::reset_var(std::string name, std::any val)
{
    if (name.empty()) return;
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
    if (vars.find(name) != vars.end())
    {
        vars[name].set_var(val);
    }
    else throw std::runtime_error("Invalid access to uninitialized variable.");
}

std::any Function::execute()
{
    if (n_ptr->type == -2) { Number *n = static_cast<Number*>(n_ptr); return n->ex(); }
    else if (n_ptr->type == -1) { Operation *op = static_cast<Operation*>(n_ptr); return op->ex(); }
    else { Variable *v = static_cast<Variable*>(n_ptr); return v->ex(); }
}


int FunctionTable::find_func(std::string name, Function *res)
{
    if (name.empty()) return 0;
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
    if (func.find(name) != func.end())
    {
        *res = func[name];
        return 1;
    }
    else return 0;
}

int FunctionTable::find_var(std::string func_name, std::string var_name, Variable *res)
{
    if (func_name.empty() || var_name.empty()) return 0;
    Function f;
    if (!find_func(func_name, &f)) return 0;
    return f.find_var(var_name, res);
}
