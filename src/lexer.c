

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
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


void showTokens(Token *head){

    const char *tokenNames[] = {
        "ID", "BREAK", "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN", "STRUCT", "VOID", "WHILE",
        "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING",
        "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "END",
        "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT", "ASSIGN", "EQUAL", "NOTEQ", "LESS", 
        "LESSEQ", "GREATER", "GREATEREQ"
    };
    Token *curr;
    curr=head;
    while(curr!=NULL){
        fprintf(stdout,"At Line %d found %s\n",curr->line,tokenNames[curr->code]);
        curr=curr->next;
    }
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
            if(isalpha(ch) || ch=='_'){pStartCh=pCrtCh; pCrtCh++; state=24;}
            else if(ch=='0'){pStartCh=pCrtCh; pCrtCh++; state=1;}
            else if(ch<='9' && ch>='1'){pStartCh=pCrtCh; pCrtCh++; state=2;}
            else if(ch=='\''){pStartCh=pCrtCh; pCrtCh++; state=19;}
            else if(ch=='"'){pStartCh=pCrtCh; pCrtCh++; state=22;}
            else if(ch=='/'){pCrtCh++; state=42;}//we don t need pStartCh here because we don t need to track linecoments info
            else if(ch=='&'){pCrtCh++; state=44;}
            else if(ch=='|'){pCrtCh++; state=46;}
            else if(ch=='='){pCrtCh++; state=49;}
            else if(ch=='!'){pCrtCh++; state=48;}
            else if(ch=='<'){pCrtCh++; state=54;}
            else if(ch=='>'){pCrtCh++; state=57;}
            else if(ch==' ' || ch=='\r' || ch=='\t'){pCrtCh++;}
            else if(ch=='\n'){line++; pCrtCh++;}
            else if(ch=='\0' || ch==EOF){addTk(END); return END;}
            else if(ch==','){pCrtCh++; addTk(COMMA); return COMMA;}
            else if(ch==';'){pCrtCh++; addTk(SEMICOLON); return SEMICOLON;}
            else if (ch == '(') { pCrtCh++; addTk(LPAR); return LPAR;}
            else if (ch == ')') { pCrtCh++; addTk(RPAR); return RPAR;}
            else if (ch == '[') { pCrtCh++; addTk(LBRACKET); return LBRACKET;}
            else if (ch == ']') { pCrtCh++; addTk(RBRACKET); return RBRACKET;}
            else if (ch == '{') { pCrtCh++; addTk(LACC); return LACC;}
            else if (ch == '}') { pCrtCh++; addTk(RACC); return RACC;}
            else if (ch == '+') { pCrtCh++; addTk(ADD); return ADD;}
            else if (ch == '-') { pCrtCh++; addTk(SUB); return SUB;}
            else if (ch == '*') { pCrtCh++; addTk(MUL); return MUL;}
            else if (ch == '.') { pCrtCh++; addTk(DOT); return DOT;}
            else {tkerr(addTk(END),"invalid character ! ");}
            break;
        case 24:
            if(isalnum(ch) || ch=='_'){pCrtCh++;}
            else{
                int len=pCrtCh-pStartCh;
                if(len==5 && memcmp(pStartCh,"break",5)==0) tk=addTk(BREAK);
                else if (len == 4 && memcmp(pStartCh, "char", 4)==0) tk = addTk(CHAR);
                else if (len == 6 && memcmp(pStartCh, "double", 6)==0) tk = addTk(DOUBLE);
                else if (len == 4 && memcmp(pStartCh, "else", 4)==0) tk = addTk(ELSE);
                else if (len == 3 && memcmp(pStartCh, "for", 3)==0) tk = addTk(FOR);
                else if (len == 2 && memcmp(pStartCh, "if", 2)==0) tk = addTk(IF);
                else if (len == 3 && memcmp(pStartCh, "int", 3)==0) tk = addTk(INT);
                else if (len == 6 && memcmp(pStartCh, "return", 6)==0) tk = addTk(RETURN);
                else if (len == 6 && memcmp(pStartCh, "struct", 6)==0) tk = addTk(STRUCT);
                else if (len == 4 && memcmp(pStartCh, "void", 4)==0) tk = addTk(VOID);
                else if (len == 5 && memcmp(pStartCh, "while", 5)==0) tk = addTk(WHILE);
                else{
                    tk = addTk(ID);
                    tk->text=createString(pStartCh,pCrtCh);
                }
                return tk->code;
            }
            break;
        case 1:
            if(ch=='.'){pCrtCh++; state=10;}
            else if(ch=='x' || ch=='X'){pCrtCh++; state=5;}
            else if(ch>='0' && ch<='7'){pCrtCh++; state=4;}
            else if(ch=='e' || ch=='E'){pCrtCh++; state=12;}
            else {state=3;}
            break;
        case 2:
            if(ch>='0' && ch<='9'){pCrtCh++;}
            else if(ch=='e' || ch=='E'){pCrtCh++; state=12;}
            else if(ch=='.'){pCrtCh++; state=10;}
            else{state=3;}
            break;
        case 3:
            tk=addTk(CT_INT);
            char *nr_int=createString(pStartCh,pCrtCh);
            tk->i=strtol(nr_int,NULL,0);
            free(nr_int);
            return tk->code;
        case 4:
            if(ch>='0' && ch<='7'){pCrtCh++;}
            else{state=3;}
            break;
        case 5:
            if(isxdigit(ch)){pCrtCh++; state=6;}
            else{tkerr(addTk(END),"invalid hex ! ");}
            break;
        case 6:
            if(isxdigit(ch)){pCrtCh++;}
            else{state = 3;}
            break;
        case 10:
            if(ch>='0' && ch<='9'){pCrtCh++; state=11;}
            else{tkerr(addTk(END),"invalid real exponent ! ");}
            break;
        case 11:
            if(ch>='0' && ch<='9'){pCrtCh++;}
            else if(ch=='e' || ch=='E'){pCrtCh++; state=12;}
            else{state=15;}
            break;
        case 12:
            if(ch>='0' && ch<='9'){pCrtCh++; state=14;}
            else if(ch=='+' || ch=='-'){pCrtCh++; state=13;}
            else{tkerr(addTk(END),"invalid real exponent");}
            break;
        case 13:
            if(ch>='0' && ch<='9'){pCrtCh++; state=14;}
            else tkerr(addTk(END), "invalid real exponent");
            break;
        case 14:
            if(ch>='0' && ch<='9'){pCrtCh++;}
            else{state=15;}
            break;
        case 15:
            tk=addTk(CT_REAL);
            char *nr_real=createString(pStartCh,pCrtCh);
            tk->r=strtol(nr_real,NULL,0);
            free(nr_real);
            return tk->code;
        case 19:
            if(ch=='//'){pCrtCh++; state=100;}//special state that isn t in the diagram for \n \a \r \... 
            //else if(ch=='\'' || ch==)
            break;
        default:
            break;
        }
    }
}

