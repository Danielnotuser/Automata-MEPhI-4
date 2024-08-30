#include <iostream>
#include "type.h"

int VariableTable::find_var(std::string name, Variable *res)
{
    if (name.empty()) return 0;
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
    if (vars.empty()) return 0;
    if (vars.find(name) != vars.end())
    {
        *res = vars[name];
        return 1;
    }
    else return 0;
}

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

void Arguments::reset_all(std::vector<std::any> v)
{
    if (v.size() != vars.size()) throw std::invalid_argument("Incorrect number of arguments when calling a function.");
    for (size_t i = 0; i < v.size(); i++)
    {
        try {vars[i].set_var(v[i]);}
        catch (std::bad_any_cast) { throw std::invalid_argument("Called arguments for the function does not match their types."); }
    }
}

void VariableTable::insert(const Arguments &a)
{
    for (size_t i = 0; i < a.vars.size(); i++)
    {
        std::pair<std::string, Variable> el = std::make_pair(a.vars[i].get_name(), a.vars[i]);
        vars.insert(el);
    }
}

std::any Function::execute()
{
    if (!var_tab.empty()) var_tab.clear();
    if (!args.vars.empty()) var_tab.insert(args);
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
