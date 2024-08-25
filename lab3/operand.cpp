#include <iostream>
#include <any>
#include "operand.h"

std::any Operand::binary(int op_type, const Operand &op2) const
{
    if (type == 0 || type == 6)
    {
        int a = std::any_cast<int>(val);
        if (op2.type == 0 || op2.type == 6)
        {
            int b = std::any_cast<int>(op2.val);
            switch (op_type) {
                case '+':
                    return a + b;
                case '-':
                    return a - b;
                case '*':
                    return a * b;
                case '/':
                    return a / b;
                case '%':
                    return a % b;
                case 'C':
                    return a <=> b;
                case 'E':
                    return a == b;
            }
        }
        else {
            std::cerr << "Error! Incorrect use of operation. Different types usage.";
            return 0;
        }
    }
    else if (type == 1 || type == 8)
    {
        int *a1 = std::any_cast<int*>(val);
        if (op2.type == 0 || op2.type == 6) {
            int b1 = std::any_cast<int>(op2.val);
            if (b1 == 0 && op_type == 'E') return bool(a1);
            switch (op_type) {
                case '+':
                    return a1 + b1;
                case '-':
                    return a1 - b1;
                default:
                    std::cerr << "Error! Invalid operator for pointer value.";
                    return 0;
            }
        }
        else if (op2.type == 1 || op2.type == 8)
        {
            int *a2 = std::any_cast<int*>(op2.val);
            if (op_type == 'E') return a1 == a2;
            else { std::cerr << "Error! Invalid operator for pointer value."; return 0; }
        }
        else {
            std::cerr << "Error! Invalid operand for pointer value.";
            return 0;
        }
    }
    else
    {
        std::cerr << "Error! Type with " << type << " id does not support operation with it.";
        return 0;
    }
    return 0;
}

std::any Operand::operator+(const Operand & op)
{
    return binary('+', op);
}

std::any Operand::operator-(const Operand &op)
{
    return binary('-', op);
}

std::any Operand::operator*(const Operand &op)
{
    return binary('*', op);
}

std::any Operand::operator/(const Operand &op)
{
    return binary('/', op);
}

std::any Operand::operator%(const Operand &op)
{
    return binary('%', op);
}


std::strong_ordering Operand::operator<=>(const Operand &op) const
{
    return std::any_cast<std::strong_ordering>(binary('C', op));
}

bool Operand::operator==(const Operand &op) const
{
    return std::any_cast<bool>(binary('E', op));
}

Operand::Operand(Node* p)
{
    if (p->type >= 0)
    {
        Variable *v = static_cast<Variable*>(p);
        type = v->type;
        val = v->ex();
    }
    else if (p->type == -2)
    {
        Number *n = static_cast<Number*>(p);
        type = 0;
        val = n->ex();
    }
    else
    {
        Operation *o = static_cast<Operation*>(p);
        val = o->ex();
        type = 0;
        if (val.type() == typeid(int))
            type = 0;
        else if (val.type() == typeid(int*))
            type = 1;

    }
}

Operand &Operand::refresh(Node *p)
{
    Operand new_op(p);
    type = new_op.type;
    val = new_op.val;
    return *this;
}

void Operand::print(const char *prompt)
{
    if (type == 0 || type == 7)
        std::cout << prompt << std::any_cast<int>(val) << std::endl;
    else
        std::cout << prompt << "Error! Invalid \"print\" argument. Only type value is required.";
}