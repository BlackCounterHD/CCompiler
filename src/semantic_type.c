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
    initSymbols(&s->args);  
    return s; 
}

Symbol   *addFuncArg(Symbol *func,const char *name,Type type) {   
    Symbol  *a=addSymbol(&func->args,name,CLS_VAR);
    a->type=type;   
    return a;
}

/*
this 2 functions above are used for predifined functions like put_s , get_s etc...
s=addExtFunc("put_s",createType(TB_VOID,-1));
addFuncArg(s,"s",createType(TB_CHAR,0));
*/