#include <iostream>
#include <cstdarg>
#include <vector>
#include "type.h"
#include "operand.h"
#include "y.tab.h"

Operation::Operation(Function *f, VariableTable *glob, VariableTable *func, int id, NodeArr n) : id(id), func_vars(func), glob_vars(glob), f(f)
{
    type = -1;
    std::vector<Node*> v = n.nodes;
    nops = v.size();
    op = new Node*[nops];
    for (int i = 0; i < nops; i++)
        op[i] = v[i];

}

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

std::any Operation::pointer_deref(Node *p, Node *expr)
{
    std::any val;
    if (expr != nullptr)
    {
        Operand o(expr);
        val = o.get_val();
        if (val.type() != typeid(int)) throw std::invalid_argument("Assigning a value that does not match a pointer.");
    }

    Variable *v = static_cast<Variable*>(p);
    if (func_vars->find_var(v->get_name(), v) || glob_vars->find_var(v->get_name(), v))
        return v->pointer_deref(val);
    else throw std::runtime_error("Dereferencing uninitialized variable.");
}

std::any Operation::arr_elref(Node *arr, Node *ind_node, Node *expr_node)
{
    Variable *v = static_cast<Variable*>(arr);
    Operand ind_op(ind_node);
    std::any ind_any = ind_op.get_val();
    if (ind_any.type() != typeid(int)) throw std::invalid_argument("Invalid index for array [] operator: index type can not be casted to int.");
    int ind = std::any_cast<int>(ind);
    std::any expr;
    if (expr_node) { Operand expr_op(expr_node); expr = expr_op.get_val(); }
    if (func_vars->find_var(v->get_name(), v) || glob_vars->find_var(v->get_name(), v))
        return v->arr_ref(ind, expr);
    else throw std::runtime_error("Invalid access to an uninitialized variable.");
}

std::any Operation::address(Node *p)
{
    Variable *v = static_cast<Variable*>(p);
    if (func_vars->find_var(v->get_name(), v) || glob_vars->find_var(v->get_name(), v))
        return v->get_addr();
    else throw std::runtime_error("Invalid access to an uninitialized variable.");
}

Variable *Operation::ref_name(Node *p)
{
    Variable *v = static_cast<Variable*>(p);
    if (!func_vars->find_var(v->get_name(), v) && !glob_vars->find_var(v->get_name(), v))
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

void call_return(int r_t, std::any ret)
{
    try {
        switch (r_t) {
            case 0: { int res = std::any_cast<int>(ret); throw res;}
            case 1: { int *res = std::any_cast<int*>(ret); throw res;}
            case 2: case 9:  { std::vector<int> res = std::any_cast<std::vector<int>>(ret); throw res;}
            case 3: case 10: { std::vector<int*> res = std::any_cast<std::vector<int*>>(ret); throw res;}
            case 4: case 11: { std::vector<std::vector<int>> res = std::any_cast<std::vector<std::vector<int>>>(ret); throw res;}
            case 5: case 12: { std::vector<std::vector<int*>> res = std::any_cast<std::vector<std::vector<int*>>>(ret); throw res;}
            case 6: { const int res = std::any_cast<const int>(ret); throw res;}
            case 7: { const int *res = std::any_cast<const int*>(ret); throw res;}
            case 8: { int *const res = std::any_cast<int*const>(ret); throw res;}
        }
    }
    catch (const std::bad_any_cast &e)
    {
        throw std::invalid_argument("Return value does not match return type.");
    }
}

void Operation::call_func()
{
    Operand arg;
    std::vector<std::any> values;
    for (int i = 0; i < nops; i++)
    {
        arg.refresh(op[i]);
        values.push_back(arg.get_val());
    }
    f->update_args(values);
    f->execute();
}

std::any Operation::ex()
{
    Operand inner;
    switch (id)
    {
        case VALUE:          { Variable *v = static_cast<Variable*>(op[0]); Operation *o = new Operation;
                                if (nops > 1)  {  if (op[1]->type == -1) o = static_cast<Operation*>(op[1]);
                                                  if (o->id == ADDR) { Variable *v2 = ref_name(o->op[0]); v->pointer_init(v2); }
                                                  else { inner.refresh(op[1]); v->init(inner.get_val()); } }
                                else v->init(); if (func_vars->find_var(v->get_name(), v)) throw std::runtime_error("Initialization of already initialized value.");
                                func_vars->insert(*v); return 0; }
        case NAME:           { Variable *v = ref_name(op[0]); return v->ex(); }
        case ASSIGN:         { reset_var(op[0], inner.refresh(op[1]).get_val()); return 0; }
        case ASDER:          { pointer_deref(op[0], op[1]); return 0; }
        case ASARR:          { arr_elref(op[0], op[1], op[2]); return 0; }
        case DEREF:          { return pointer_deref(op[0]); }
        case ADDR:           { Variable *v = ref_name(op[0]); return v; }
        case LSQUARE:        { return arr_elref(op[0], op[1]); }
        case SIZE:           { Variable *v = ref_name(op[0]); return v->get_size(); }
        case FUNC:           { try { call_func(); } catch (std::any &a) { return a; }}
        case RETURN:         { std::any ret = inner.refresh(op[0]).get_val(); int r_t = std::any_cast<int>(inner.refresh(op[1]).get_val()); call_return(r_t, ret); }
        case WHILE:          { bool br = false; while(Operand(op[0]) != Operand(0, 0)) {
                                    try { inner.refresh(op[1]); }
                                    catch (const std::string &e) { if (e == "break") {br = true; break;} throw;} } if (nops > 2 && !br) inner.refresh(op[2]); return 0; }
        case BREAK:          throw "break";
        case IF:             if (inner.refresh(op[0]) != Operand(0, 0)) inner.refresh(op[1]);
                                else if (nops > 2) inner.refresh(op[2]); return 0;
        case ZERO:           if (Operand(op[0]) == Operand(0, 0)) inner.refresh(op[1]); return 0;
        case NOTZERO:        if (Operand(op[0]) != Operand(0, 0)) inner.refresh(op[1]); return 0;
        case PRINT:          inner.refresh(op[0]); inner.print("Output: "); return 0;
        case SEMI:           if (!nops) return 0; inner.refresh(op[0]); inner.refresh(op[1]); return 0;
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
        case TOP:            return rob->top();
        case BOTTOM:         return rob->bottom();
        case LEFT:           return rob->left();
        case RIGHT:          return rob->right();
        case PORTAL:         rob->portal(); return 0;
        case TELEPORT:       rob->teleport(); return 0;
    }
    return 0;
}