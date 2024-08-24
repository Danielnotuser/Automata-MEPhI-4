#include <iostream>
#include <cstdarg>
#include <vector>
#include "type.h"
#include "y.tab.h"
#include "operand.h"

std::any execute(std::string f_name, Node *p)
{
    if (p->type == -2) { Number *n = static_cast<Number*>(p); return n->ex(); }
    else if (p->type == -1) { Operation *op = static_cast<Operation*>(p); return op->ex(); }
    else { Variable *v = static_cast<Variable*>(p); return v->ex(); }
}

Operation::Operation(int id, int nops, ...) : id(id), nops(nops)
{
    type = -1;
    va_list ap;
    va_start(ap, nops);
    op = new Node*[nops];
    if (!op) throw std::runtime_error("Out of memory.");
    for (int i = 0; i < nops; i++)
        op[i] = va_arg(ap, Node*);
    va_end(ap);
}

std::any deref(Node *p)
{
    switch (p->type)
    {
        case 1: { int *a = std::any_cast<int*>(static_cast<Variable*>(p)->ex()); return *a; }
        case 7: { const int *b = std::any_cast<const int*>(static_cast<Variable*>(p)->ex()); return *b; }
        case 8: { int *const c = std::any_cast<int*const>(static_cast<Variable*>(p)->ex()); return *c; }
        default: std::cerr << "Error! Dereferencing a non-pointer variable."; return 0;
    }
    return 0;
}

std::any address(Node *p)
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

std::any arr_el_ref(Node *var_node, Node *ind_node)
{
    if (var_node->type <= 1 || (var_node->type >= 6 && var_node->type <= 8)) { std::cerr << "Error! Reference by index to the element of array went wrong: variable is not an array."; return 0; }
    Variable *var = static_cast<Variable*>(var_node);
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

std::any Operation::ex()
{
    Operand inner;
    switch (id)
    {
        case yy::parser::token::WHILE:          while(Operand(op[0])) inner.refresh(op[1]); if (nops > 2) inner.refresh(op[2]); return 0;
        case yy::parser::token::IF:             if (Operand(op[0])) inner.refresh(op[1]); else if (nops > 2) inner.refresh(op[2]); return 0;
        case yy::parser::token::ZERO:           if (Operand(op[0]) == 0) inner.refresh(op[1]); return 0;
        case yy::parser::token::NOTZERO:        if (Operand(op[0]) != 0) inner.refresh(op[1]); return 0;
        case yy::parser::token::PRINT:          inner.refresh(op[0]); inner.print("Output: "); return 0;
        case yy::parser::token::SEMI:           inner.refresh(op[0]); inner.refresh(op[1]); return 0;
        case yy::parser::token::ASSIGN:         if (op[0]->type >= 0) {Variable *v = std::any_cast<Variable*>(op[0]); v->set_var(Operand(op[1]).get_val());}
                                    else std::cerr << "Error! Um.. lvalue of assignment is not a variable..";
        case yy::parser::token::DEREF:          return deref(op[0]);
        case yy::parser::token::ADDR:           return address(op[0]);
        case yy::parser::token::LSQUARE:        return arr_el_ref(op[0], op[1]);
        case yy::parser::token::UMINUS:         return Operand(0, 0)  - Operand(op[0]);
        case yy::parser::token::PLUS:           return Operand(op[0]) + Operand(op[1]);
        case yy::parser::token::MINUS:          return Operand(op[0]) - Operand(op[1]);
        case yy::parser::token::STAR:           return Operand(op[0]) * Operand(op[1]);
        case yy::parser::token::SLASH:          return Operand(op[0]) / Operand(op[1]);
        case yy::parser::token::PERC:           return Operand(op[0]) + Operand(op[1]);
        case yy::parser::token::LESS:           return Operand(op[0]) < Operand(op[1]);
        case yy::parser::token::GREATER:        return Operand(op[0]) > Operand(op[1]);
        case yy::parser::token::GE:             return Operand(op[0]) >= Operand(op[1]);
        case yy::parser::token::LE:             return Operand(op[0]) <= Operand(op[1]);
        case yy::parser::token::NE:             return Operand(op[0]) != Operand(op[1]);
        case yy::parser::token::EQ:             return Operand(op[0]) == Operand(op[1]);
    }
    return 0;
}