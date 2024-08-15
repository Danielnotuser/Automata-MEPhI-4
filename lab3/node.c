#include <stdio.h>
#include "type.h"
#include "y.tab.h"

int ex(nodeType *p)
{
    if (!p) return 0;
    switch (p->type)
    {
        case typeNum: return p->num.value;
        case typeVar: return sym[p->var.i];
        case typeOpr:
            switch (p->opr.oper)
            {
                case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); return 0;
                case IF:        if (ex(p->opr.op[0])) ex(p->opr.op[1]);
                                else if(p->opr.num_ops > 2) ex(p->opr.op[2]);
                case PRINT:     printf("%d\n", ex(p->opr.op[0])); return 0;
                case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
                case '=':       return sym[p->opr.op[0]->var.i] = ex(p->opr.op[1]);
                case UMINUS:    return -ex(p->opr.op[0]);
                case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
                case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
                case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
                case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
                case '<':       return ex(p->opr.op[0]) < ex(p->opr.op[1]);
                case '>':       return ex(p->opr.op[0]) > ex(p->opr.op[1]);
                case GE:       return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
                case LE:       return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
                case NE:       return ex(p->opr.op[0]) != ex(p->opr.op[1]);
                case EQ:       return ex(p->opr.op[0]) == ex(p->opr.op[1]);
            }
    }
    return 0;
}