#ifndef TYPE_H
#define TYPE_H

typedef enum { typeNum, typeVar, typeOpr } nodeEnum;

typedef struct {
    int value;
} numNodeType;

typedef struct MyMap {
    char **var_name;
    int *var_value;
    int var_num;
} MyMap;

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

extern MyMap vars;

#endif