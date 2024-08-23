#ifndef TYPE_H
#define TYPE_H

typedef enum { typeNum, typeVar, typeOpr } nodeEnum;

typedef struct {
    int value;
} numNodeType;

typedef struct {
    char *name;
} varNodeType;

typedef struct {
    int oper;
    int num_ops;
    struct nodeTypeTag *op[1];
} oprNodeType;

typedef struct nodeTypeTag{
    nodeEnum type;
    union {
        numNodeType num;
        varNodeType var;
        oprNodeType opr;
    };
} nodeType;

typedef struct Elem {
    int type;
    void *var;
} Elem;

typedef struct VarMap {
    char **var_names;
    Elem *var_info;
    int var_num;
} VarMap;

typedef struct Func {
    int ret_type;
    int *arg_types;
    int arg_num;
    VarMap vars;
    nodeType *tree;
} Func;

typedef struct FuncMap {
    char **names;
    Func *func;
    int func_num;
} FuncMap;

extern VarMap global_vars;

#endif