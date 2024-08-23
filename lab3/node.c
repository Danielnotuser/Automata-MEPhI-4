#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "type.h"
#include "y.tab.h"

void add_var(int type, char *str, int val)
{
    global_vars.var_num++;
    global_vars.var_info = (Elem*) realloc(global_vars.var_info, global_vars.var_num * sizeof(Elem));
    global_vars.var_names = (char**) realloc(global_vars.var_names, global_vars.var_num * sizeof(char*));
    global_vars.var_names[global_vars.var_num - 1] = strdup(str);
    Elem e;
    e.type = 0;
    e.var = (void*) &val;
    global_vars.var_info[global_vars.var_num - 1] = e;
}

char *lower_case(const char *str)
{
    char *res = strdup(str);
    for (int i = 0; res[i] != '\0'; i++)
        res[i] = tolower(res[i]);
    return res;
}

int find_var(const char *str, int *ind, int *type)
{
    char *check = lower_case(str);
    for (int i = 0; i < global_vars.var_num; i++) {
        char *var_str = lower_case(global_vars.var_names[i]);
        if (!strcmp(var_str, check)) {
            *ind = i;
            *type = global_vars.var_info[i].type;
            free(var_str);
            free(check);
            return 1;
        }
        free(var_str);

    }
    free(check);
    return 0;
}

int ex(nodeType *p)
{
    if (!p) return 0;
    switch (p->type)
    {
        case typeNum: return p->num.value;
        case typeVar: int ind, type; if (find_var(p->var.name, &ind, &type)) return vars.var_value[ind]; else fprintf(stderr, "Error! The variable is not initialized.\n"); return 0;
        case typeOpr:
            switch (p->opr.oper)
            {
                case VALUE:     int ind1; if (!find_var(p->opr.op[1]->var.name, &ind1)) add_var(p->opr.op[0], p->opr.op[1]->var.name, ex(p->opr.op[1])); else fprintf(stderr, "Error! The variable has already been initialized.\n"); return 0;
                case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); if (p->opr.num_ops > 2) ex(p->opr.op[2]); return 0;
                case IF:        if (ex(p->opr.op[0])) ex(p->opr.op[1]);
                                else if(p->opr.num_ops > 2) ex(p->opr.op[2]); return 0;
                case ZERO:      if (ex(p->opr.op[0]) == 0) ex(p->opr.op[1]); return 0;
                case NOTZERO:   if (ex(p->opr.op[0]) != 0) ex(p->opr.op[1]); return 0;
                case PRINT:     printf("Output: %d\n", ex(p->opr.op[0])); return 0;
                case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
                case '=':       int as_ind; if (find_var(p->opr.op[0]->var.name, &as_ind)) return vars.var_value[as_ind] = ex(p->opr.op[1]); else fprintf(stderr, "Error! The variable is not initialized.\n"); return 0;
                case UMINUS:    return -ex(p->opr.op[0]);
                case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
                case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
                case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
                case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
                case '%':       return ex(p->opr.op[0]) % ex(p->opr.op[1]);
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