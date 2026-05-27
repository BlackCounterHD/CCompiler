#include <stdio.h>
#include <string.h>
#include "../header/semantic_domain_w_vector.h"
#include "../header/utils.h"

int crtDepth=0;

void initSymbols(Symbols *symbols){
    symbols->begin=NULL;
    symbols->end=NULL;
    symbols->after=NULL;
}

Symbol *newSymbol(const char *name,int cls){

    Symbol *s;

    SAFEALLOC(s,Symbol)

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