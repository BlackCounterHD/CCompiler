

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
    exit(1);
}

void tkerr(const Token *tk,const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(1);
}

char *createString(const char *pStart, const char *pEnd){
    int length=pEnd-pStart;
    char *str=(char *)malloc(length+1);
    if(!str){
        fprintf(stderr,"couldn t allocate memory\n");
        exit(1);
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
        "ID", "BREAK", "CHAR", "FLOAT", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN", "STRUCT", "VOID", "WHILE",
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

void showTokensInFile(Token *head){

    FILE *f=fopen("bin/lexer_output.txt","w");
    if(!f){
        fprintf(stderr,"error at opening the file");
        exit(1);
    }
    const char *tokenNames[] = {
        "ID", "BREAK", "CHAR", "FLOAT", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN", "STRUCT", "VOID", "WHILE",
        "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING",
        "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "END",
        "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT", "ASSIGN", "EQUAL", "NOTEQ", "LESS", 
        "LESSEQ", "GREATER", "GREATEREQ"
    };
    Token *curr;
    curr=head;
    while(curr!=NULL){
        fprintf(f,"At Line %d found %s\n",curr->line,tokenNames[curr->code]);
        curr=curr->next;
    }
    fclose(f);
}

int getNextToken(){
    Token *tk;
    const char *pStartCh;
    char ch;
    int state=0;

    char buffer[4096];
    int buffI=0;

    /*
        When i have char *c = "a\n12" I want \n to bee stored in buffer
        with the ASCII code of ENTER that means it ll ocupy only one byte.
        If i tried storing it with create string it will hold 2 bytes
        one for \ and one for n and also i ll need to store it like
        tk->text = createString(pStartCh+1,pCrtCh-1) to skip ""
    */

    while(1){
        ch=*(pCrtCh);
        switch(state)
        {
        case 0:
            if(isalpha(ch) || ch=='_'){pStartCh=pCrtCh; pCrtCh++; state=24;}
            else if(ch=='0'){pStartCh=pCrtCh; pCrtCh++; state=1;}
            else if(ch<='9' && ch>='1'){pStartCh=pCrtCh; pCrtCh++; state=2;}
            else if(ch=='\''){pStartCh=pCrtCh; pCrtCh++; state=19;}
            else if(ch=='"'){pStartCh=pCrtCh; pCrtCh++; state=22; buffI=0;}
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
        case 3:  //CT_INT 
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
            else{tkerr(addTk(END),"invalid fractional part ! ");}
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
        case 15: //CT_REAL
            tk=addTk(CT_REAL);
            char *nr_real=createString(pStartCh,pCrtCh);
            tk->r=atof(nr_real);
            free(nr_real);
            return tk->code;
        case 19:
            if(ch=='\\'){pCrtCh++; state=100;}//special state that isn t in the diagram for \n \a \r \...  EXPLICITLY : char c = '\n';
            else if(ch=='\'' || ch=='\n' || ch==0){tkerr(addTk(END),"Invalid char");}
                    /*
                        char c = '
                        a'; this is wrong
                    */
            else {tk=addTk(CT_CHAR); tk->i=(unsigned char)ch; pCrtCh++; state=20;}
            break;
        case 20:
            if(ch=='\''){pCrtCh++; state=21;}
            else{tkerr(addTk(END),"missing closing quote for character constant");}
            break;
        case 21:
            return CT_CHAR;
        case 22:
            if(ch=='\\'){pCrtCh++; state=101;}//special state that isn t in the diagram for \n \a \r etc..
            else if(ch=='\n' || ch==0){tkerr(addTk(END),"Invalid char");}
            else if(ch=='"'){
                tk=addTk(CT_STRING); 
                char *str=(char *)malloc(buffI+1);
                if(!str){fprintf(stderr,"error at allocating memory for str");exit(1);}
                strncpy(str,buffer,buffI);
                str[buffI]='\0';
                tk->text=str;
                pCrtCh++;
                state=23;
            }
            else {buffer[buffI]=ch; buffI++; pCrtCh++;}
            break;
        case 23:
            return tk->code;
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
                else if (len == 5 && memcmp(pStartCh, "float", 5)==0) tk = addTk(FLOAT);
                else{
                    tk = addTk(ID);
                    tk->text=createString(pStartCh,pCrtCh);
                }
                return tk->code;
            }
            break;
        case 42:
            if(ch=='/'){state=61; pCrtCh++;}
            else state=60;
            break;
        case 44:
            if(ch=='&'){state=45; pCrtCh++;}
            else{tkerr(addTk(END),"wrong character after &");}
            break;
        case 45:
            addTk(AND); 
            return AND;
        case 46:
            if(ch=='|'){state=47; pCrtCh++;}
            else{tkerr(addTk(END),"wrong character after |");}
            break;
        case 47:
            addTk(OR);
            return OR;
        case 48:
            if(ch=='='){state=53; pCrtCh++;}
            else{state=52;}
            break;
        case 49:
            if(ch=='='){state=51; pCrtCh++;}
            else{state=50;}
            break;
        case 50:
            addTk(ASSIGN);
            return ASSIGN;
        case 51:
            addTk(EQUAL);
            return EQUAL;
        case 52:
            addTk(NOT);
            return NOT;
        case 53:
            addTk(NOTEQ);
            return NOTEQ;
        case 54:
            if(ch=='='){state=56; pCrtCh++;}
            else{state=55;}
            break;
        case 55:
            addTk(LESS);
            return LESS;
        case 56:
            addTk(LESSEQ);
            return LESSEQ;
        case 57:
            if(ch=='='){state=59; pCrtCh++;}
            else{state=58;}
            break;
        case 58:
            addTk(GREATER);
            return GREATER;
        case 59:
            addTk(GREATEREQ);
            return GREATEREQ;
        case 60:
            addTk(DIV);
            return DIV;
        case 61:
            if(ch=='\n' || ch=='\0' || ch=='\r'){state=0;} //without pCrtCh++ because at state 0 we check again for the same \n and increment the line and pCrtCh
            else{pCrtCh++;}
            break;
        case 100:{
            int escaped;
                switch (ch) {
                    case 'a': escaped = '\a'; break;
                    case 'b': escaped = '\b'; break;
                    case 'f': escaped = '\f'; break;
                    case 'n': escaped = '\n'; break;
                    case 'r': escaped = '\r'; break;
                    case 't': escaped = '\t'; break;
                    case 'v': escaped = '\v'; break;
                    case '\'': escaped = '\''; break;
                    case '?': escaped = '?'; break;
                    case '"': escaped = '"'; break;
                    case '\\': escaped = '\\'; break;
                    case '0': escaped = '\0'; break;
                    default:
                        tkerr(addTk(END), "invalid escape sequence: \\%c", ch);
                        escaped = 0;
                }
                tk = addTk(CT_CHAR);
                tk->i = escaped;
                pCrtCh++;
                state = 20; 
                break;
            }
        case 101:{
            char escaped;
                switch (ch) {
                    case 'a': escaped = '\a'; break;
                    case 'b': escaped = '\b'; break;
                    case 'f': escaped = '\f'; break;
                    case 'n': escaped = '\n'; break;
                    case 'r': escaped = '\r'; break;
                    case 't': escaped = '\t'; break;
                    case 'v': escaped = '\v'; break;
                    case '\'': escaped = '\''; break;
                    case '?': escaped = '?'; break;
                    case '"': escaped = '"'; break;
                    case '\\': escaped = '\\'; break;
                    case '0': escaped = '\0'; break;
                    default:
                        tkerr(addTk(END), "invalid escape sequence: \\%c", ch);
                        escaped = 0;
                }
                buffer[buffI]=escaped;
                buffI++;
                pCrtCh++;
                state=22;
            break;
            }
        } 
    }
}

