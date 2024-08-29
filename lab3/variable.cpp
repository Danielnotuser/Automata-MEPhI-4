#include <iostream>
#include <vector>
#include "type.h"

void Variable::pointer_init(Variable *v)
{
    size = 0;
    exp = 4;
    switch (type) {
        case 1:
            { if (v->type == 6) throw std::invalid_argument("Assigning a constant value to a pointer of a non-constant type.");
            std::any &an = v->get_val();
            int a = std::any_cast<int>(an);
            val = &a; break; }
        case 7: case 8:
            val = v->get_addr();
            break;
        default:
            throw std::runtime_error("Assigning an address to a variable that is not a pointer.");
    }
}

void Variable::init(std::any value)
{
    size = 0;
    exp = 4;
    if (type >= 0 && type <= 5)
    {
        switch (type) {
            case 0:
                { int c0; size = 1;
                if (value.has_value()) c0 = std::any_cast<int>(value);
                val = c0;
                break; }
            case 1:
                {val = new int; size = 1;
                if (value.has_value()) val = std::any_cast<int*>(value);
                break;}
            case 2:
                {if (value.has_value()) exp = std::any_cast<int>(value);
                val = std::vector<int>(exp);
                break;}
            case 3:
                {if (value.has_value()) exp = std::any_cast<int>(value);
                val = std::vector<int *>(exp);
                break;}
            case 4:
                {if (value.has_value()) exp = std::any_cast<int>(value);
                val = std::vector<std::vector<int>>(exp, std::vector<int>(exp));
                break;}
            case 5:
                {if (value.has_value()) exp = std::any_cast<int>(value);
                val = std::vector<std::vector<int *>>(exp, std::vector<int *>(exp));
                break;}
            default:
                throw std::runtime_error("Default initialization of an object of const type.");
        }
    }
    else if (value.has_value())
    {
        switch (type) {
            case 6:
                val = std::any_cast<const int>(value); size = 1;
                break;
            case 7:
                val = std::any_cast<const int*>(value); size = 1;
                break;
            case 8:
                val = std::any_cast<int*const>(value); size = 1;
                break;
            case 9:
                {exp = std::any_cast<int>(value);
                std::vector<int> c10(exp);
                val = c10;
                break;}
            case 10:
                {exp = std::any_cast<int>(value);
                std::vector<int*> c11(exp);
                val = c11;
                break;}
            case 11:
                {exp = std::any_cast<int>(value);
                std::vector<std::vector<int>> c12(exp, std::vector<int>(exp));
                val = c12;
                break;}
            case 12:
                {exp = std::any_cast<int>(value);
                val = std::vector<std::vector<int *>>(exp, std::vector<int *>(exp));
                break;}
        }
    }
    else throw std::runtime_error("Invalid const variable initialization.");
}

Variable::Variable(const Variable &v)
{
    type = v.type;
    size = v.size;
    exp = v.exp;
    name = v.name;
    val = v.val;
}

Variable &Variable::operator=(const Variable &v)
{
    type = v.type;
    size = v.size;
    exp = v.exp;
    name = v.name;
    val = v.val;
    return *this;
}

Variable &Variable::set_var(std::any an)
{
    if (type >= 6 && type != 8) std::cerr << "Error! Assignment of the const variable.";
    else
    {
        switch (type)
        {
            case 0:
                if (an.type() == typeid(int)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 1:
                if (an.type() == typeid(int*)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 2:
                if (an.type() == typeid(std::vector<int>)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 3:
                if (an.type() == typeid(std::vector<int*>)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 4:
                if (an.type() == typeid(std::vector<std::vector<int>>)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 5:
                if (an.type() == typeid(std::vector<std::vector<int*>>)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 7:
                if (an.type() == typeid(const int*)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
        }
    }
    return *this;
}

std::any Variable::arr_ref(int ind, std::any expr)
{
    if (type < 2 || (type > 5 && type < 9)) throw std::invalid_argument("No match for ‘operator[]’ for non-array types.");
    if (ind > size)
        throw std::invalid_argument("Invalid index for array ‘operator[]’: index is greater than the size.");
    else if (ind == size && type >= 9)
        throw std::invalid_argument("Invalid index for array ‘operator[]’: constant-size arrays do not support indices equal to their size.");
    switch (type)
    {
        case 2: case 9:     { std::vector<int> &vect = std::any_cast<std::vector<int>&>(val);
                                if (!expr.has_value()) { if (ind == size) throw std::runtime_error("Invalid access to uninitialized element of array"); return vect[ind]; }
                                if (expr.type() == typeid(int)) { if (ind == size) size++; vect[ind] = std::any_cast<int>(expr); return 0; }
                                else throw std::invalid_argument("Assigning value of invalid type to the array element."); }
        case 3: case 10:    { std::vector<int*> &vect = std::any_cast<std::vector<int*>&>(val);
                                if (!expr.has_value()) { if (ind == size) throw std::runtime_error("Invalid access to uninitialized element of array"); return vect[ind]; }
                                if (expr.type() == typeid(int*)) { if (ind == size) size++; vect[ind] = std::any_cast<int*>(expr); return 0; }
                                else throw std::invalid_argument("Assigning value of invalid type to the array element."); }
        case 4: case 11:    { std::vector<std::vector<int>> &vect = std::any_cast<std::vector<std::vector<int>>&>(val);
                                if (!expr.has_value()) { if (ind == size) throw std::runtime_error("Invalid access to uninitialized element of array"); return vect[ind]; }
                                if (expr.type() == typeid(std::vector<int>)) { if (ind == size) size++; vect[ind] = std::any_cast<std::vector<int>>(expr); return 0; }
                                else throw std::invalid_argument("Assigning value of invalid type to the array element."); }
        case 5: case 12:    { std::vector<std::vector<int*>> &vect = std::any_cast<std::vector<std::vector<int*>>&>(val);
                                if (!expr.has_value()) { if (ind == size) throw std::runtime_error("Invalid access to uninitialized element of array"); return vect[ind]; }
                                if (expr.type() == typeid(std::vector<int*>)) { if (ind == size) size++; vect[ind] = std::any_cast<std::vector<int*>>(expr); return 0; }
                                else throw std::invalid_argument("Assigning value of invalid type to the array element."); }
    }
    return 0;
}

std::any Variable::pointer_deref(std::any expr)
{
    switch (type)
    {
        case 1:     { int *ptr = std::any_cast<int*>(val); if (!expr.has_value()) return *ptr;
                        if (expr.type() == typeid(int)) *ptr = std::any_cast<int>(expr);
                        else throw std::invalid_argument("Assigning value to different type.");
                    }
        case 7:     { const int *ptr = std::any_cast<const int*>(val); if (!expr.has_value()) return *ptr;
                        else throw std::invalid_argument("Constant-valued pointers do not support assignment outside of initialization.");;
                    }
        case 8:     { int *const ptr = std::any_cast<int*const>(val); if (!expr.has_value()) return *ptr;
                        if (expr.type() == typeid(int)) *ptr = std::any_cast<int>(expr);
                        else throw std::invalid_argument("Assigning value to different type.");
                    }
        default:  throw std::invalid_argument("Dereferencing a non-pointer variable.");
    }
}

int *Variable::get_addr()
{
    switch (type)
    {
        case 0: {
            int a = std::any_cast<int>(val);
            return &a;
        }
        default: throw std::invalid_argument("The address can only be taken from variables of (const) value type.");
    }
}

const int *Variable::get_const_addr()
{
    switch (type)
    {
        case 6: {
            const int a = std::any_cast<const int>(val);
            return &a;
        }
        default: throw std::invalid_argument("The address can only be taken from variables of (const) value type.");
    }
}
