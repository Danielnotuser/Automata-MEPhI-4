typedef enum { typeNum, typeVar, typeOpr } nodeEnum;

typedef struct {
    int value;
} numNodeType;

typedef struct {
    int i;
} varNodeType;

typedef struct {
    int oper;
    int num_ops;
    struct nodeTypeTag *op[1];
} oprNodeType;

typedef struct {
    nodeEnum type;
    union {
        numNodeType num;
        varNodeType var;
        oprNodeType opr;
    };
} nodeType;

extern int sym[26];