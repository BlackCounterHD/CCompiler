#ifndef LEXER_H
#define LEXER_H


enum {
    ID, BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE,
    CT_INT, CT_REAL, CT_CHAR, CT_STRING,
    COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, END,
    ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, 
    LESSEQ, GREATER, GREATEREQ
};


typedef struct _Token {
    int code;
    union {
        char *text; // folosit pentru ID si CT_STRING
        long int i; // folosit pentru CT_INT si CT_CHAR
        double r;   // folosit pentru CT_REAL
    };
    int line;
    struct _Token *next;
} Token;


extern Token *head;
extern Token *tail;
extern const char *pCrtCh;
extern int line;


void err(const char *fmt, ...);
void tkerr(const Token *tk, const char *fmt, ...);
char *createString(const char *pStart, const char *pEnd);
Token *addTk(int code);
int getNextToken();
void showTokens(Token *head);
void freeTokenList(Token *head);

#endif