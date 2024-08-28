#include <iostream>
#include <cstdarg>
#include <vector>
#include "type.h"
#include "operand.h"
#include "y.tab.h"

Operation::Operation(VariableTable *glob, VariableTable *func, int id, int nops, ...) : id(id), nops(nops), func_vars(func), glob_vars(glob)
{
    type = -1;
    va_list ap;
    va_start(ap, nops);
    op = new Node*[nops];
    for (int i = 0; i < nops; i++)
        op[i] = va_arg(ap, Node*);
    va_end(ap);
}

Operation::Operation(VariableTable *func, int id, int nops, ...) : id(id), nops(nops), func_vars(func), glob_vars(nullptr)
{
    type = -1;
    va_list ap;
    va_start(ap, nops);
    op = new Node*[nops];
    for (int i = 0; i < nops; i++)
        op[i] = va_arg(ap, Node*);
    va_end(ap);
}

Operation::Operation(int id, int nops, ...) : id(id), nops(nops), func_vars(nullptr), glob_vars(nullptr)
{
    type = -1;
    va_list ap;
    va_start(ap, nops);
    op = new Node*[nops];
    for (int i = 0; i < nops; i++)
        op[i] = va_arg(ap, Node*);
    va_end(ap);
}

std::any deref(Variable *p)
{
    switch (p->type)
    {
        case 1: { int *a = std::any_cast<int*>(static_cast<Variable*>(p)->ex()); return *a; }
        case 7: { const int *b = std::any_cast<const int*>(static_cast<Variable*>(p)->ex()); return *b; }
        case 8: { int *const c = std::any_cast<int*const>(static_cast<Variable*>(p)->ex()); return *c; }
        default: std::cerr << "Error! Dereferencing a non-pointer variable."; return 0;
    }
}

void Operation::assign_deref(Node *p, Node *expr)
{
    Operand o(expr);
    std::any val = o.get_val();
    if (val.type() != typeid(int)) throw std::invalid_argument("Assigning a value that does not match a pointer.");
    Variable *v = static_cast<Variable*>(p);
    if (func_vars->find_var(v->get_name(), v))
    {
        switch (v->type)
        {
            case 1: { int *ptr = std::any_cast<int*>(v->ex()); *ptr = std::any_cast<int>(val); func_vars->reset_var(v->get_name(), ptr); return;}
            case 7:   throw std::invalid_argument("Changing value of the pointer const value.");
            case 8: { int *const ptr = std::any_cast<int*const>(v->ex()); *ptr = std::any_cast<int>(val); func_vars->reset_var(v->get_name(), ptr); return; }
            default: throw std::invalid_argument("Dereferencing a non-pointer variable.");
        }
    }
    else if (glob_vars->find_var(v->get_name(), v))
    {
        switch (v->type)
        {
            case 1: { int *ptr = std::any_cast<int*>(v->ex()); *ptr = std::any_cast<int>(val); glob_vars->reset_var(v->get_name(), ptr); return;}
            case 7:   throw std::invalid_argument("Changing value of the \"pointer const value\".");
            case 8: { int *const ptr = std::any_cast<int*const>(v->ex()); *ptr = std::any_cast<int>(val); glob_vars->reset_var(v->get_name(), ptr); return; }
            default: throw std::invalid_argument("Dereferencing a non-pointer variable.");
        }
    }
    else throw std::runtime_error("Dereferencing uninitialized variable.");
}

void Operation::assign_arr(Node *arr, Node *ind_node, Node *expr_node)
{
    Variable *v = static_cast<Variable*>(arr);
    Operand ind_op(ind_node);
    std::any ind_any = ind_op.get_val();
    Operand expr_op(expr_node);
    std::any expr = expr_op.get_val();
    if (ind_any.type() != typeid(int)) throw std::invalid_argument("Invalid index for array [] operator: index type can not be casted to int.");
    int ind = std::any_cast<int>(ind);
    if (func_vars->find_var(v->get_name(), v))
    {
        if (ind > v->get_size()) throw std::invalid_argument("Invalid index for array [] operator: index is greater than size.");
        switch (v->type)
        {
            case 2: { std::vector<int> vect = std::any_cast<std::vector<int>>(v->ex());
                        if (expr.type() == typeid(int)) vect[ind] = std::any_cast<int>(expr);
                        else throw std::invalid_argument("Assigning value of invalid type to the array element."); func_vars->reset_var(v->get_name(), vect); }
            case 3: { std::vector<int*> vect = std::any_cast<std::vector<int*>>(v->ex());
                        if (expr.type() == typeid(int*)) vect[ind] = std::any_cast<int*>(expr);
                        else throw std::invalid_argument("Assigning value of invalid type to the array element.");  func_vars->reset_var(v->get_name(), vect); }
            case 4: { std::vector<std::vector<int>> vect = std::any_cast<std::vector<std::vector<int>>>(v->ex());
                        if (expr.type() == typeid(std::vector<int>)) vect[ind] = std::any_cast<std::vector<int>>(expr);
                        else throw std::invalid_argument("Assigning value of invalid type to the array element.");  func_vars->reset_var(v->get_name(), vect); }
            case 5: { std::vector<std::vector<int*>> vect = std::any_cast<std::vector<std::vector<int*>>>(v->ex());
                        if (expr.type() == typeid(std::vector<int*>)) vect[ind] = std::any_cast<std::vector<int*>>(expr);
                        else throw std::invalid_argument("Assigning value of invalid type to the array element.");  func_vars->reset_var(v->get_name(), vect); }
            case 9: case 10: case 11: throw std::invalid_argument("Assignment to the const array element.");
        }
    }
    else if (glob_vars->find_var(v->get_name(), v))
    {
        switch (v->type)
        {
            case 2: { std::vector<int> vect = std::any_cast<std::vector<int>>(v->ex());
                if (expr.type() == typeid(int)) vect[ind] = std::any_cast<int>(expr);
                else throw std::invalid_argument("Assigning value of invalid type to the array element."); glob_vars->reset_var(v->get_name(), vect); }
            case 3: { std::vector<int*> vect = std::any_cast<std::vector<int*>>(v->ex());
                if (expr.type() == typeid(int*)) vect[ind] = std::any_cast<int*>(expr);
                else throw std::invalid_argument("Assigning value of invalid type to the array element."); glob_vars->reset_var(v->get_name(), vect); }
            case 4: { std::vector<std::vector<int>> vect = std::any_cast<std::vector<std::vector<int>>>(v->ex());
                if (expr.type() == typeid(std::vector<int>)) vect[ind] = std::any_cast<std::vector<int>>(expr);
                else throw std::invalid_argument("Assigning value of invalid type to the array element."); glob_vars->reset_var(v->get_name(), vect); }
            case 5: { std::vector<std::vector<int*>> vect = std::any_cast<std::vector<std::vector<int*>>>(v->ex());
                if (expr.type() == typeid(std::vector<int*>)) vect[ind] = std::any_cast<std::vector<int*>>(expr);
                else throw std::invalid_argument("Assigning value of invalid type to the array element."); glob_vars->reset_var(v->get_name(), vect); }
            case 9: case 10: case 11: throw std::invalid_argument("Assignment to the const array element.");
        }
    }
}

std::any address(Variable *p)
{
    switch (p->type)
    {
        case 0: {int a0 = std::any_cast<int>(static_cast<Variable*>(p)->ex()); return &a0;}
        case 1: {int *a1 = std::any_cast<int*>(static_cast<Variable*>(p)->ex()); return &a1;}
        case 2: {std::vector<int> a2 = std::any_cast<std::vector<int>>(static_cast<Variable*>(p)->ex()); return &a2;}
        case 3: {std::vector<int*> a3 = std::any_cast<std::vector<int*>>(static_cast<Variable*>(p)->ex()); return &a3;}
        case 4: {std::vector<std::vector<int>> a4 = std::any_cast<std::vector<std::vector<int>>>(static_cast<Variable*>(p)->ex()); return &a4;}
        case 5: {std::vector<std::vector<int*>> a6 = std::any_cast<std::vector<std::vector<int*>>>(static_cast<Variable*>(p)->ex()); return &a6;}
        case 6: {const int a7 = std::any_cast<const int>(static_cast<Variable*>(p)->ex()); return &a7;}
        case 7: {const int *a8 = std::any_cast<const int*>(static_cast<Variable*>(p)->ex()); return &a8;}
        case 8: {int*const a9 = std::any_cast<int*const>(static_cast<Variable*>(p)->ex()); return &a9;}
        case 9: {std::vector<int> a10 = std::any_cast<std::vector<int>>(static_cast<Variable*>(p)->ex()); return &a10;}
        case 10: {std::vector<int*> a11 = std::any_cast<std::vector<int*>>(static_cast<Variable*>(p)->ex()); return &a11;}
        case 11: {std::vector<std::vector<int>> a12 = std::any_cast<std::vector<std::vector<int>>>(static_cast<Variable*>(p)->ex()); return &a12;}
    }
    return 0;
}

std::any Operation::arr_el_ref(Variable *var, Node *ind_node)
{
    if (var->type <= 1 || (var->type >= 6 && var->type <= 8)) { std::cerr << "Error! Reference by index to the element of array went wrong: variable is not an array."; return 0; }
    std::any any_ind = Operand(ind_node).get_val();
    if (any_ind.type() != typeid(int) && any_ind.type() != typeid(const int)) { std::cerr << "Error! Invalid index to get array element."; return 0;}
    int ind = std::any_cast<int>(any_ind);
    switch (var->type)
    {
        case 2: { std::vector<int> v1 = std::any_cast<std::vector<int>>(var->ex()); return v1[ind]; }
        case 3: { std::vector<int*> v2 = std::any_cast<std::vector<int*>>(var->ex()); return v2[ind]; }
        case 4: { std::vector<std::vector<int>> v3 = std::any_cast<std::vector<std::vector<int>>>(var->ex()); return v3[ind]; }
        case 5: { std::vector<std::vector<int*>> v4 = std::any_cast<std::vector<std::vector<int*>>>(var->ex()); return v4[ind]; }
        case 9: { const std::vector<int> v5 = std::any_cast<const std::vector<int>>(var->ex()); return v5[ind]; }
        case 10: { const std::vector<int*> v6 = std::any_cast<const std::vector<int*>>(var->ex()); return v6[ind]; }
        case 11: { const std::vector<std::vector<int>> v7 = std::any_cast<const std::vector<std::vector<int>>>(var->ex()); return v7[ind]; }
    }
    return 0;
}

Variable *Operation::ref_name(Node *p)
{
    Variable *v = static_cast<Variable*>(p);
    if (!func_vars->find_var(v->get_name(), v))
        if (!glob_vars->find_var(v->get_name(), v))
            throw std::runtime_error("Invalid access to an uninitialized variable.");
    return v;
}

void Operation::reset_var(Node *p, std::any val)
{
    if (p->type < 0) Operand o(p);
    else
    {
        Variable *v = static_cast<Variable*>(p);
        if (func_vars->find_var(v->get_name(), v))
            func_vars->reset_var(v->get_name(), val);
        else if (glob_vars->find_var(v->get_name(), v))
            glob_vars->reset_var(v->get_name(), val);
        else throw std::runtime_error("Invalid access to an uninitialized variable.");
    }
}

std::any Operation::ex()
{
    Operand inner;
    switch (id)
    {
        case VALUE:          { Variable *v = static_cast<Variable*>(op[0]); if (nops > 1) { inner.refresh(op[1]); v->init(inner.get_val()); }
                                else v->init(); if (func_vars->find_var(v->get_name(), v)) throw std::runtime_error("Initialization of already initialized value.");
                                func_vars->insert(*v); return 0; }
        case NAME:           { Variable *v = ref_name(op[0]); return v->get_val(); }
        case ASSIGN:         { reset_var(op[0], inner.refresh(op[1]).get_val()); return 0; }
        case ASDER:          { assign_deref(op[0], op[1]); return 0; }
        case ASARR:          { reset_var(op[0], inner.refresh(op[1]).get_val()); return 0; }
        case DEREF:          { Variable *v = ref_name(op[0]); return deref(v); }
        case ADDR:           { Variable *v = ref_name(op[0]); return address(v); }
        case LSQUARE:        { Variable *v = ref_name(op[0]); return arr_el_ref(v, op[1]); }
        case WHILE:          { bool br = false; while(Operand(op[0]) != Operand(0, 0)) {
                                    try { inner.refresh(op[1]); }
                                    catch (const std::string &e) { if (e == "break") {br = true; break;} } } if (nops > 2 && !br) inner.refresh(op[2]); return 0; }
        case BREAK:          throw "break";
        case IF:             if (inner.refresh(op[0]) != Operand(0, 0)) inner.refresh(op[1]);
                                else if (nops > 2) inner.refresh(op[2]); return 0;
        case ZERO:           if (Operand(op[0]) == Operand(0, 0)) inner.refresh(op[1]); return 0;
        case NOTZERO:        if (Operand(op[0]) != Operand(0, 0)) inner.refresh(op[1]); return 0;
        case PRINT:          inner.refresh(op[0]); inner.print("Output: "); return 0;
        case SEMI:           inner.refresh(op[0]); inner.refresh(op[1]); return 0;
        case UMINUS:         return Operand(0, 0)  - Operand(op[0]);
        case PLUS:           return Operand(op[0]) + Operand(op[1]);
        case MINUS:          return Operand(op[0]) - Operand(op[1]);
        case STAR:           return Operand(op[0]) * Operand(op[1]);
        case SLASH:          return Operand(op[0]) / Operand(op[1]);
        case PERC:           return Operand(op[0]) % Operand(op[1]);
        case LESS:           if (Operand(op[0]) < Operand(op[1])) return 1; else return 0;
        case GREATER:        if (Operand(op[0]) > Operand(op[1])) return 1; else return 0;
        case GE:             if (Operand(op[0]) >= Operand(op[1])) return 1; else return 0;
        case LE:             if (Operand(op[0]) <= Operand(op[1])) return 1; else return 0;
        case NE:             if (Operand(op[0]) != Operand(op[1])) return 1; else return 0;
        case EQ:             if (Operand(op[0]) == Operand(op[1])) return 1; else return 0;
    }
    return 0;
}