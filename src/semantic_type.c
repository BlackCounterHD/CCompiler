#include <stdio.h>
#include <stdlib.h>
#include "../header/semantic_type.h"
#include "../header/syntactical.h"

void cast(Type *dst,Type *src){
    if(src->nElements>-1){
        if(dst->nElements>-1){
            if(src->typeBase!=dst->typeBase){
                tkerr(crtTk,"an array cannot be converted to an array of another type");
            }
        }
        else{
            tkerr(crtTk,"an array cannot be converted to a non-array");
        }
    }
    else{
        if(dst->nElements>-1){
            tkerr(crtTk,"a non-array cannot be converted to an array");
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
                return;
            }
        break;
        case TB_STRUCT:
            if(dst->typeBase==TB_STRUCT){
                if(src->s!=dst->s){
                    tkerr(crtTk,"a structure cannot be converted to another one");
                }
                return;
            }
        break;
    }
    tkerr(crtTk,"incompatible types");
}