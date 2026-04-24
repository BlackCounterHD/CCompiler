

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "../header/lexer.h"

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");


Token *tail=NULL;
Token *head=NULL;
const char *pCrtCh;
int line=1;

Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk,Token);
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
    if(head==NULL){
        head=tk; //it also works like tail=tk; head=tail or head=tail=tk;
        tail=tk;
    }
    else{
        tail->next=tk;
        tail=tk;
    }
    return tk;
}

void err(const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk,const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

char *createString(const char *pStart, const char *pEnd){
    int length=pEnd-pStart;
    char *str=(char *)malloc(length+1);
    if(!str){
        fprintf(stderr,"couldn t allocate memory");
        exit(0);
    }
    strncpy(str,pStart,length);
    str[length]='\0';
    return str;
}
/*
void showTokens(Token *head){
    Token* curr;
    curr=head;
    while(curr!=NULL){

    }
}*/

void freeTokenList(Token *head){
    Token *curr1,*curr2;
    curr1=curr2=head;
    while(curr2!=NULL){
        curr2=curr2->next;
        free(curr1);
        curr1=curr2;
    }
    free(curr2);
    head=NULL;
    tail=NULL;
}

int getNextToken(){
    Token *tk;
    const char *pStartCh;
    char ch;
    int state=0,nCh;

    while(1){
        ch=*(pCrtCh);
        switch(state)
        {
        case 0:
            /* code */
            break;
        
        default:
            break;
        }
    }
}