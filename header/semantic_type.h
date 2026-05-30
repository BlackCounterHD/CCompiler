#ifndef SEMANTICT_H
#define SEMANTICT_H

#include "../header/semantic_domain_w_vector.h"

Type   createType(int typeBase,int nElements){   
    Type  t;   
    t.typeBase=typeBase;   
    t.nElements=nElements;   
    return t; 
}

typedef union 
{
    long int i; //int,char
    double d; //double
    float f; //float
    const char *str; //char[]
}CtVal; //constant value

typedef struct 
{
    Type type; //type of the result
    int isLVal; //if it is a LVal
    int isCtVal; //if it is a constant value
    CtVal ctVal; //the constant value
}RetVal; //used for exprAdd | exprCast | exprPrimary

void cast(Type *dst,Type *src);
Type getArithType(Type *s1,Type *s2);
Symbol *addExtFunc(const char *name,Type type);
Symbol *addFuncArg(Symbol *func,const char *name,Type type);


#endif