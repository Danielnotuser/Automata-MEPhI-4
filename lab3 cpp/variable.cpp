#include <iostream>
#include <vector>
#include <cstdarg>
#include "type.h"

Variable::Variable(int type, std::string name, std::any value) : name(name)
{
    this->type = type;
    size = 1;
    exp = 4;
    if (type >= 0 && type <= 6)
    {
        switch (type) {
            case 0:
                int c0;
                if (value.has_value()) c0 = std::any_cast<int>(value);
                var = c0;
                break;
            case 1:
                int *c1 = new int;
                if (value.has_value()) c1 = std::any_cast<int*>(value);
                var = c1;
                break;
            case 2:
                if (value.has_value()) exp = std::any_cast<int>(value);
                std::vector<int> c2(exp);
                var = c2;
                break;
            case 3:
                if (value.has_value()) exp = std::any_cast<int>(value);
                std::vector<int *> c3(exp);
                var = c3;
                break;
            case 4:
                if (value.has_value()) exp = std::any_cast<int>(value);
                std::vector<std::vector<int>> c4(exp, std::vector<int>(exp));
                var = c4;
                break;
            case 5:
                if (value.has_value()) exp = std::any_cast<int>(value);
                std::vector<std::vector<int *>> c6(exp, std::vector<int *>(exp));
                var = c6;
                break;
            default:
                throw std::runtime_error("Default initialization of an object of const type.");
        }
    }
    else if (value.has_value())
    {
        switch (var_type) {
            case 6:
                const int c7 = std::any_cast<int>(value);
                var = c7;
                break;
            case 7:
                int *const c8 = std::any_cast<int*>(value);
                var = c8;
                break;
            case 8:
                const int* c9 = std::any_cast<int*>(value);
                var = c9;
                break;
            case 9:
                exp = std::any_cast<int>(value);
                const std::vector<int> c10(exp);
                var = c10;
                break;
            case 10:
                exp = std::any_cast<int>(value);
                const std::vector<int*> c11(exp);
                var = c11;
                break;
            case 11:
                exp = std::any_cast<int>(value);
                const std::vector<std::vector<int>> c12(exp, std::vector<int>(exp));
                var = c12;
                break;
        }
    }
    else throw std::runtime_error("Invalid const variable initialization.");
}

Variable::Variable(const Variable &v)
{
    size = v.size;
    exp = v.exp;
    name = v.name;
    var = v.var;
}

Variable &Variable::set_var(std::any an)
{
    if (type >= 6 && type != 8) std::cerr << "Error! Assignment of the const variable.";
    else
    {
        switch (type)
        {
            case 0:
                if (an.type() == typeid(int)) var = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 1:
                if (an.type() == typeid(int*)) var = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 2:
                if (an.type() == typeid(std::vector<int>)) var = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 3:
                if (an.type() == typeid(std::vector<int*>)) var = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 4:
                if (an.type() == typeid(std::vector<std::vector<int>>)) var = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 5:
                if (an.type() == typeid(std::vector<std::vector<int*>>)) var = an; else std::cerr << "Error! Assignment to value of the different type."; break;
            case 8:
                if (an.type() == typeid(const int*)) var = an; else std::cerr << "Error! Assignment to value of the different type."; break;
        }
    }
    return *this;
}
