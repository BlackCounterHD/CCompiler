#ifndef SEMANTICT_H
#define SEMANTICT_H

#include "../header/semantic_domain_w_vector.h"

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

int convTo(Type *dst,Type *src); //cast
int arithTypeTo(Type *s1,Type *s2,Type *rez); //getArithType
Symbol *addExtFunc(const char *name,Type type);
Symbol *addFuncArg(Symbol *func,const char *name,Type type);
Type createType(int typeBase,int nElements);
int canBeScalar(RetVal *r);
void addExtFuncs();


#endif