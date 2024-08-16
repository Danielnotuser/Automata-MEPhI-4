#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "y.tab.h"

void add_var(char *str, int val)
{
    vars.var_num++;
    vars.var_value = (int*) realloc(vars.var_value, vars.var_num * sizeof(int));
    vars.var_name = (char**) realloc(vars.var_name, vars.var_num * sizeof(char*));
    vars.var_name[vars.var_num] = str;
    vars.var_value[vars.var_num] = val;
}

int find_var(char *str, int *ind)
{
    for (int i = 0; i < vars.var_num; i++)
        if (!strcmp(vars.var_name[i], str))
        {
            *ind = i;
            return 1;
        }
    return 0;
}

int ex(nodeType *p)
{
    if (!p) return 0;
    switch (p->type)
    {
        case typeNum: return p->num.value;
        case typeVar: int ind; if (find_var(p->var.name, &ind)) return vars.var_value[ind]; else fprintf(stderr, "Error! Variable is not initialized."); return 0;
        case typeOpr:
            switch (p->opr.oper)
            {
                case VALUE:     add_var(p->opr.op[0]->var.name, ex(p->opr.op[1]));
                case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); ex(p->opr.op[2]); return 0;
                case IF:        if (ex(p->opr.op[0])) ex(p->opr.op[1]);
                                else if(p->opr.num_ops > 2) ex(p->opr.op[2]);
                case PRINT:     printf("%d\n", ex(p->opr.op[0])); return 0;
                case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
                case '=':       int as_ind; if (find_var(p->opr.op[0]->var.name, &as_ind)) return vars.var_value[as_ind] = ex(p->opr.op[1]); else fprintf(stderr, "Error! Variable is not initialized."); return 0;
                case UMINUS:    return -ex(p->opr.op[0]);
                case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
                case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
                case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
                case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
                case '%':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
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