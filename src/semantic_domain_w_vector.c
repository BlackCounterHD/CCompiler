#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../header/semantic_domain_w_vector.h"
#include "../header/utils.h"

int crtDepth=0;
Symbols symbols;
Symbol *owner=NULL;
int globalMemoryOffset=0;

void initSymbols(Symbols *symbols){
    symbols->begin=NULL;
    symbols->end=NULL;
    symbols->after=NULL;
}

Symbol *newSymbol(const char *name,int cls){

    Symbol *s;

    SAFEALLOC(s,Symbol)

    if (cls == CLS_FUNC || cls == CLS_EXTFUNC) {
        initSymbols(&s->fn.params);
        initSymbols(&s->fn.locals);
    } else if (cls == CLS_STRUCT) {
        initSymbols(&s->members);
    }
    s->name=name;
    s->cls=cls;

    return s;
}

Symbol *addSymbolToDomain(Symbols *symbols,const char *name,int cls){
    Symbol *s;
    if(symbols->end==symbols->after){ // create more room

        int count=symbols->after-symbols->begin;
        int n=count*2; // double the room

        if(n==0)n=1; // needed for the initial case

        symbols->begin=(Symbol**)realloc(symbols->begin, n*sizeof(Symbol*));

        if(symbols->begin==NULL)err("not enough memory");

        symbols->end=symbols->begin+count;
        symbols->after=symbols->begin+n;

    }

    s=newSymbol(name,cls);

    *symbols->end++=s;
    s->depth=crtDepth;

    return s;
}

Symbol *findSymbol(Symbols *symbols,const char *name){

    int count=symbols->end-symbols->begin;
    for(int i=count-1;i>=0;i--){
        if(strcmp(symbols->begin[i]->name,name)==0){
            return symbols->begin[i];
        }
    }
    return NULL;
}

Symbol *findSymbolInDomain(Symbols *symbols,const char *name){

    int currDepth=crtDepth;
    currDepth--;
    int count=symbols->end-symbols->begin;
    count--;

    while(count>=0 && symbols->begin[count]->depth>currDepth){
        if(strcmp(symbols->begin[count]->name,name)==0){
            return symbols->begin[count];
        }
        count--;
    }

    return NULL;
}


void pushDomain(){
    crtDepth++;
}

void dropDomain(Symbols *symbols){

    crtDepth--;

    int count=symbols->end-symbols->begin;

    count--;

    while(count>=0 && symbols->begin[count]->depth>crtDepth){
        free(symbols->begin[count]);
        symbols->end--;
        count--;
    }
}

int symbolsLen(Symbols args){
    return args.end-args.begin;
}

void addSymbolToList(Symbols *argsOrmembers,Symbol *d){

    if(argsOrmembers->end==argsOrmembers->after){
        int count=argsOrmembers->after-argsOrmembers->begin;
        int n=count*2; // double the room

        if(n==0)n=1; // needed for the initial case

        argsOrmembers->begin=(Symbol**)realloc(argsOrmembers->begin, n*sizeof(Symbol*));

        if(argsOrmembers->begin==NULL)err("not enough memory");

        argsOrmembers->end=argsOrmembers->begin+count;
        argsOrmembers->after=argsOrmembers->begin+n;
    }

    *argsOrmembers->end++=d;
}

Symbol *findSymbolInList(Symbols *argsOrmembers,const char *name){
    int count=argsOrmembers->end-argsOrmembers->begin;
    for(int i=count-1;i>=0;i--){
        if(strcmp(argsOrmembers->begin[i]->name,name)==0){
            return argsOrmembers->begin[i];
        }
    }
    return NULL;
}

Symbol *dupSymbol(Symbol *d){

    Symbol *s;

    SAFEALLOC(s,Symbol);

    *s=*d;

    return s;
}

int typeSize(Type *type){

    int nr=type->nElements;
    if(nr==-1){
        nr=1;
    }
    if(type->typeBase==TB_INT){return sizeof(int)*nr;}
    else if(type->typeBase==TB_CHAR){return sizeof(char)*nr;}
    else if(type->typeBase==TB_DOUBLE){return sizeof(double)*nr;}
    else if(type->typeBase==TB_FLOAT){return sizeof(float)*nr;}
    else if(type->typeBase==TB_STRUCT){
        int count=type->s->members.end-type->s->members.begin;
        int size=0;
        for(int i=0;i<count;i++){
            size=size+typeSize(&type->s->members.begin[i]->type);
        }
        size=size*nr;
        return size;
    }
    return 0;
}

int allocInGlobalMemory(int size){
    int currentOffset = globalMemoryOffset;
    
    globalMemoryOffset += size;
    
    return currentOffset;
}
