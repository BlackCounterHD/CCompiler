#include <stdio.h>
#include <stdlib.h>
#include "../header/semantic_type.h"
#include "../header/syntactical.h"
#include "../header/semantic_domain_w_vector.h"

int convTo(Type *dst,Type *src){
    if(src->nElements>-1){
        if(dst->nElements>-1){
            if(src->typeBase!=dst->typeBase){
                //tkerr(crtTk,"an array cannot be converted to an array of another type");
                return 0;
            }
        }
        else{
            //tkerr(crtTk,"an array cannot be converted to a non-array");
            return 0;
        }
    }
    else{
        if(dst->nElements>-1){
            //tkerr(crtTk,"a non-array cannot be converted to an array");
            return 0;
        }
    }
    switch(src->typeBase){    
        case TB_CHAR:    
        case TB_INT:    
        case TB_DOUBLE:
        case TB_FLOAT:
            switch(dst->typeBase){      
                case TB_CHAR:      
                case TB_INT:      
                case TB_DOUBLE:
                case TB_FLOAT:       
                return 1;
            }
        break;
        case TB_STRUCT:
            if(dst->typeBase==TB_STRUCT){
                if(src->s!=dst->s){
                    //tkerr(crtTk,"a structure cannot be converted to another one");
                    return 0;
                }
                return 1;
            }
        break;
    }
    //tkerr(crtTk,"incompatible types");
    return 0;
} //it s the same function as cast but with different name and type to satisfy "AtomC - analiza de tipuri pdf"

Symbol   *addExtFunc(const char *name,Type type) { 

    Symbol   *s=addSymbolToDomain(&symbols,name,CLS_EXTFUNC);   
    s->type=type;
    /*   i don t need those anymore because they are already initialized inside addSymbolToDomain
    initSymbols(&s->fn.locals);
    initSymbols(&s->fn.params);  
    */
    return s; 
}

Symbol   *addFuncArg(Symbol *func,const char *name,Type type) {   
    Symbol  *a=newSymbol(name,CLS_VAR);
    a->type=type;
    a->mem = MEM_ARG;
    addSymbolToList(&func->fn.params, a);   
    return a;
}

/*
this 2 functions above are used for predifined functions like put_s , get_s etc...
s=addExtFunc("put_s",createType(TB_VOID,-1));
addFuncArg(s,"s",createType(TB_CHAR,0));
*/

Type createType(int typeBase,int nElements){   
    Type  t;   
    t.typeBase=typeBase;   
    t.nElements=nElements;   
    return t; 
}

int arithTypeTo(Type *s1,Type *s2,Type *rez){

    if(s1->nElements>=0 || s2->nElements>=0){
        return 0;
    }

    if(s1->typeBase==TB_STRUCT || s2->typeBase==TB_STRUCT){
        return 0;
    }
    
    if(s1->typeBase==TB_VOID || s2->typeBase==TB_VOID){
        return 0;
    }

    rez->nElements = -1;
    rez->s = NULL;

    if(s1->typeBase==TB_DOUBLE || s2->typeBase==TB_DOUBLE){
        rez->typeBase=TB_DOUBLE;
    }
    else if(s1->typeBase==TB_FLOAT || s2->typeBase==TB_FLOAT){
        rez->typeBase=TB_FLOAT;
    }
    else {
        rez->typeBase=TB_INT;
    }
    return 1;
}

int canBeScalar(RetVal *r){
    if(r->type.nElements<0 &&(r->type.typeBase==TB_INT || r->type.typeBase==TB_CHAR || r->type.typeBase==TB_FLOAT || r->type.typeBase==TB_DOUBLE)){
        return 1;
    }
    return 0;
}


void addExtFuncs() {
    Symbol *s;

    s = addExtFunc("put_s", createType(TB_VOID, -1));
    addFuncArg(s, "s", createType(TB_CHAR, 0));

    s = addExtFunc("get_s", createType(TB_VOID, -1));
    addFuncArg(s, "s", createType(TB_CHAR, 0));

    s = addExtFunc("put_i", createType(TB_VOID, -1));
    addFuncArg(s, "i", createType(TB_INT, -1));

    s = addExtFunc("get_i", createType(TB_INT, -1));

    s = addExtFunc("put_d", createType(TB_VOID, -1));
    addFuncArg(s, "d", createType(TB_DOUBLE, -1));

    s = addExtFunc("get_d", createType(TB_DOUBLE, -1));

    s = addExtFunc("put_c", createType(TB_VOID, -1));
    addFuncArg(s, "c", createType(TB_CHAR, -1));

    s = addExtFunc("get_c", createType(TB_CHAR, -1));

    s = addExtFunc("seconds", createType(TB_DOUBLE, -1));
}