#include <stdio.h>
#include <string.h>
#include "../header/semantic_domain_w_vector.h"
#include "../header/utils.h"

Symbols symbols;
int crtDepth=0;

void initSymbols(Symbols *symbols){
    symbols->begin=NULL;
    symbols->end=NULL;
    symbols->after=NULL;
}

Symbol *addSymbol(Symbols *symbols,const char *name,int cls){
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

    SAFEALLOC(s,Symbol)
    *symbols->end++=s;
    s->name=name;
    s->cls=cls;
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

void pushDomain(){
    crtDepth++;
}

void dropDomain(Symbols *symbols){

    crtDepth--;

    int count=symbols->end-symbols->begin;

    count--;

    while(count>0 && symbols->begin[count]->depth>crtDepth){
        free(symbols->begin[count]);
        symbols->end--;
        count--;
    }
}