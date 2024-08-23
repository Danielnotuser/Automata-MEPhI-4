#ifndef REGEX_OPERAND_H
#define REGEX_OPERAND_H

#include "type.h"
#include <any>

class Operand {
private:
    int type;
    std::any val;
    std::any binary(int op_type, const Operand &op2) const;
public:
    Operand() = default;
    Operand(Node*);
    Operand(int type, std::any val) : type(type), val(val) {};

    std::any get_val() { return val; };

    Operand &refresh(Node *p);
    void print(const char *prompt);

    std::any operator+(Operand &);
    std::any operator-(Operand &);
    std::any operator*(Operand &);
    std::any operator/(Operand &);
    std::any operator%(Operand &);
    operator bool() const;
    bool operator<=>(Operand&) const;

};


#endif //REGEX_OPERAND_H
