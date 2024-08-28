#include <iostream>
#include <vector>
#include "type.h"

void Variable::init(std::any value)
{
    size = 1;
    exp = 4;
    if (type >= 0 && type <= 6)
    {
        switch (type) {
            case 0:
                { int c0;
                if (value.has_value()) c0 = std::any_cast<int>(value);
                val = c0;
                break; }
            case 1:
                {val = new int;
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
                val = std::any_cast<const int>(value);;
                break;
            case 7:
                val = std::any_cast<int*const>(value);;
                break;
            case 8:
                val = std::any_cast<const int*>(value);
                break;
            case 9:
                {exp = std::any_cast<int>(value);
                const std::vector<int> c10(exp);
                val = c10;
                break;}
            case 10:
                {exp = std::any_cast<int>(value);
                const std::vector<int*> c11(exp);
                val = c11;
                break;}
            case 11:
                {exp = std::any_cast<int>(value);
                const std::vector<std::vector<int>> c12(exp, std::vector<int>(exp));
                val = c12;
                break;}
        }
    }
    else throw std::runtime_error("Invalid const variable initialization.");
}

Variable::Variable(const Variable &v)
{
    size = v.size;
    exp = v.exp;
    name = v.name;
    val = v.val;
}

Variable &Variable::operator=(const Variable &v)
{
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
            case 8:
                if (an.type() == typeid(const int*)) val = an; else std::cerr << "Error! Assignment to value of the different type."; break;
        }
    }
    return *this;
}
