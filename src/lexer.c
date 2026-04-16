#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "../header/lexer.h"


Token *tokens = NULL;
Token *lastToken = NULL;
const char *pCrtCh = NULL;
int line = 1;

#define SAFEALLOC(var, Type) if((var=(Type*)malloc(sizeof(Type)))==NULL) err("not enough memory");


void err(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

void tkerr(const Token *tk, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "error in line %d: ", tk->line);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);
    va_end(va);
    exit(-1);
}

char *createString(const char *pStart, const char *pEnd) {
    int len = pEnd - pStart;
    char *str = (char*)malloc(len + 1);
    if (str) {
        strncpy(str, pStart, len);
        str[len] = '\0';
    }
    return str;
}

Token *addTk(int code) {
    Token *tk;
    SAFEALLOC(tk, Token);
    tk->code = code;
    tk->line = line;
    tk->next = NULL;
    if (lastToken) lastToken->next = tk;
    else tokens = tk;
    lastToken = tk;
    return tk;
}


int getNextToken() {
    int state = 0;
    char ch;
    const char *pStartCh;
    Token *tk;
    
    //Buffer pentru a construi string-uri litere cu litera si a procesa escape-urile
    char buf[4096]; 
    int bufIdx = 0;

    while (1) {
        ch = *pCrtCh;
        switch (state) {
            case 0: // HUB-UL PRINCIPAL
                if (isalpha(ch) || ch == '_') { pStartCh = pCrtCh; pCrtCh++; state = 24; }
                else if (ch == '0') { pStartCh = pCrtCh; pCrtCh++; state = 1; }
                else if (ch >= '1' && ch <= '9') { pStartCh = pCrtCh; pCrtCh++; state = 2; }
                else if (ch == '\'') { pStartCh = pCrtCh; pCrtCh++; state = 19; }
                else if (ch == '"') { pStartCh = pCrtCh; pCrtCh++; bufIdx = 0; state = 22; } // Initializam buffer-ul
                else if (ch == ',') { pCrtCh++; addTk(COMMA); return COMMA; }
                else if (ch == ';') { pCrtCh++; addTk(SEMICOLON); return SEMICOLON; }
                else if (ch == '(') { pCrtCh++; addTk(LPAR); return LPAR; }
                else if (ch == ')') { pCrtCh++; addTk(RPAR); return RPAR; }
                else if (ch == '[') { pCrtCh++; addTk(LBRACKET); return LBRACKET; }
                else if (ch == ']') { pCrtCh++; addTk(RBRACKET); return RBRACKET; }
                else if (ch == '{') { pCrtCh++; addTk(LACC); return LACC; }
                else if (ch == '}') { pCrtCh++; addTk(RACC); return RACC; }
                else if (ch == '+') { pCrtCh++; addTk(ADD); return ADD; }
                else if (ch == '-') { pCrtCh++; addTk(SUB); return SUB; }
                else if (ch == '*') { pCrtCh++; addTk(MUL); return MUL; }
                else if (ch == '.') { pCrtCh++; addTk(DOT); return DOT; }
                else if (ch == '&') { pCrtCh++; state = 44; }
                else if (ch == '|') { pCrtCh++; state = 46; }
                else if (ch == '!') { pCrtCh++; state = 52; }
                else if (ch == '=') { pCrtCh++; state = 49; }
                else if (ch == '<') { pCrtCh++; state = 54; }
                else if (ch == '>') { pCrtCh++; state = 57; }
                else if (ch == '/') { pCrtCh++; state = 60; }
                else if (ch == ' ' || ch == '\r' || ch == '\t') { pCrtCh++; } // Ignoram spatiile
                else if (ch == '\n') { line++; pCrtCh++; } // Actualizam contorul de linii
                else if (ch == '\0' || ch == EOF) { addTk(END); return END; }
                else tkerr(addTk(END), "invalid character");
                break;

            // --- ID si Keywords (Starea 24) ---
            case 24:
                if (isalnum(ch) || ch == '_') pCrtCh++;
                else {
                    int n = pCrtCh - pStartCh;
                    if (n == 5 && !memcmp(pStartCh, "break", 5)) tk = addTk(BREAK);
                    else if (n == 4 && !memcmp(pStartCh, "char", 4)) tk = addTk(CHAR);
                    else if (n == 6 && !memcmp(pStartCh, "double", 6)) tk = addTk(DOUBLE);
                    else if (n == 4 && !memcmp(pStartCh, "else", 4)) tk = addTk(ELSE);
                    else if (n == 3 && !memcmp(pStartCh, "for", 3)) tk = addTk(FOR);
                    else if (n == 2 && !memcmp(pStartCh, "if", 2)) tk = addTk(IF);
                    else if (n == 3 && !memcmp(pStartCh, "int", 3)) tk = addTk(INT);
                    else if (n == 6 && !memcmp(pStartCh, "return", 6)) tk = addTk(RETURN);
                    else if (n == 6 && !memcmp(pStartCh, "struct", 6)) tk = addTk(STRUCT);
                    else if (n == 4 && !memcmp(pStartCh, "void", 4)) tk = addTk(VOID);
                    else if (n == 5 && !memcmp(pStartCh, "while", 5)) tk = addTk(WHILE);
                    else {
                        tk = addTk(ID);
                        tk->text = createString(pStartCh, pCrtCh);
                    }
                    return tk->code;
                }
                break;

            // --- NUMERE INTREGI SI REALE ---
            case 1: // Am inceput cu '0'
                if (ch == 'x' || ch == 'X') { pCrtCh++; state = 5; } // Hex
                else if (ch >= '0' && ch <= '7') { pCrtCh++; state = 4; } // Octal
                else if (ch == '.') { pCrtCh++; state = 10; } // Real
                else if (ch == 'e' || ch == 'E') { pCrtCh++; state = 12; } // Real
                else state = 3; //(CT_INT)
                break;

            case 2: // Am inceput cu [1-9]
                if (isdigit(ch)) pCrtCh++;
                else if (ch == '.') { pCrtCh++; state = 10; }
                else if (ch == 'e' || ch == 'E') { pCrtCh++; state = 12; }
                else state = 3;
                break;

            // HEX
            case 5:
                if (isxdigit(ch)) { pCrtCh++; state = 6; }
                else tkerr(addTk(END), "invalid hex");
                break;
            case 6:
                if (isxdigit(ch)) pCrtCh++;
                else state = 3;
                break;
            
            // OCTAL
            case 4:
                if (ch >= '0' && ch <= '7') pCrtCh++;
                else state = 3;
                break;

            // FINALIZARE CT_INT (Starea 3)
            case 3:
                tk = addTk(CT_INT);
                {
                    char *temp = createString(pStartCh, pCrtCh);
                    tk->i = strtol(temp, NULL, 0); 
                    free(temp);
                }
                return tk->code;

            // ZECIMALE REAL
            case 10:
                if (isdigit(ch)) pCrtCh++;
                else if (ch == 'e' || ch == 'E') { pCrtCh++; state = 11; }
                else state = 15;
                break;

            // EXPONENT REAL (Starea 11 unita cu 12)
            case 11:
            case 12: 
                if (ch == '+' || ch == '-') { pCrtCh++; state = 13; }
                else state = 13; // Fallthrough
                break;
            case 13:
                if (isdigit(ch)) { pCrtCh++; state = 14; }
                else tkerr(addTk(END), "invalid real exponent");
                break;
            case 14:
                if (isdigit(ch)) pCrtCh++;
                else state = 15;
                break;

            // FINALIZARE CT_REAL (Starea 15)
            case 15:
                tk = addTk(CT_REAL);
                {
                    char *temp = createString(pStartCh, pCrtCh);
                    tk->r = atof(temp);
                    free(temp);
                }
                return tk->code;

            // --- CARACTERE (CT_CHAR) ---
            case 19:
                if (ch == '\\') {
                    pCrtCh++;
                    state = 58; // -> Secventa escape pt char
                }
                else if (ch == '\'' || ch == '\n' || ch == 0) {
                    tkerr(addTk(END), "invalid character constant");
                }
                else {
                    tk = addTk(CT_CHAR);
                    tk->i = (unsigned char)ch;
                    pCrtCh++;
                    state = 20; // -> Asteptam ghilimeaua de inchidere
                }
                break;

            case 58: { // Tratare escape pentru caracter
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
                state = 20; // Mergem sa asteptam inchiderea
                break;
            }

            case 20: // Finalizare CT_CHAR
                if (ch == '\'') { 
                    pCrtCh++;
                    return CT_CHAR; // Tokenul a fost creat in starea 19 sau 58
                }
                else {
                    tkerr(addTk(END), "missing closing quote for character constant");
                }
                break;

            // --- STRING-URI (CT_STRING) ---
            case 22:
                if (ch == '\\') {
                    pCrtCh++;
                    state = 59; // -> Secventa escape pt string
                }
                else if (ch == '"') { // Finalizare string
                    pCrtCh++;
                    buf[bufIdx] = '\0';
                    tk = addTk(CT_STRING);
                    tk->text = (char *)malloc(bufIdx + 1);
                    if (!tk->text) err("not enough memory");
                    memcpy(tk->text, buf, bufIdx + 1);
                    return CT_STRING;
                }
                else if (ch == 0) {
                    tkerr(addTk(END), "unterminated string constant");
                }
                else {
                    if (ch == '\n') line++;
                    buf[bufIdx++] = ch; // Punem litera in buffer
                    pCrtCh++; // Ramanem in starea 22
                }
                break;

            case 59: { // Tratare escape pentru string
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
                        tkerr(addTk(END), "invalid escape sequence in string: \\%c", ch);
                        escaped = 0;
                }
                buf[bufIdx++] = escaped; // Punem litera convertita in buffer
                pCrtCh++;
                state = 22; // Ne intoarcem la citirea normala a stringului
                break;
            }

            // --- OPERATORI LOGICI SI RELATIONALI ---
            case 44:
                if (ch == '&') { pCrtCh++; addTk(AND); return AND; }
                else tkerr(addTk(END), "invalid &");
                break;
            case 46:
                if (ch == '|') { pCrtCh++; addTk(OR); return OR; }
                else tkerr(addTk(END), "invalid |");
                break;
            case 49:
                if (ch == '=') { pCrtCh++; addTk(EQUAL); return EQUAL; }
                else { addTk(ASSIGN); return ASSIGN; }
            case 52:
                if (ch == '=') { pCrtCh++; addTk(NOTEQ); return NOTEQ; }
                else { addTk(NOT); return NOT; }
            case 54:
                if (ch == '=') { pCrtCh++; addTk(LESSEQ); return LESSEQ; }
                else { addTk(LESS); return LESS; }
            case 57:
                if (ch == '=') { pCrtCh++; addTk(GREATEREQ); return GREATEREQ; }
                else { addTk(GREATER); return GREATER; }

            // --- COMENTARII SI DIV ---
            case 60:
                if (ch == '/') { pCrtCh++; state = 61; } // E comentariu pe linie
                else { addTk(DIV); return DIV; } // Era doar divizare
                break;
            case 61: // Bucla LINECOMMENT
                if (ch == '\n' || ch == '\r' || ch == '\0') state = 0; // Gata comentariul
                else pCrtCh++;
                break;
        }
    }
}

// Afisarea formatata a tokenilor
void showTokens() {
    Token *tk = tokens;
    
    
    const char *tokenNames[] = {
        "ID", "BREAK", "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN", "STRUCT", "VOID", "WHILE",
        "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING",
        "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "END",
        "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT", "ASSIGN", "EQUAL", "NOTEQ", "LESS", 
        "LESSEQ", "GREATER", "GREATEREQ"
    };

    printf("--- LISTA DE TOKENI ---\n");
    while (tk != NULL) {
        printf("Line %2d | %-10s", tk->line, tokenNames[tk->code]);
        
        if (tk->code == ID) {
            printf(" : %s", tk->text);
        } else if (tk->code == CT_STRING) {
            printf(" : ");
            // Parcurgem string-ul si "traducem" caracterele invizibile la afisare
            for (int i = 0; tk->text[i] != '\0'; i++) {
                if (tk->text[i] == '\n') printf("\\n");
                else if (tk->text[i] == '\t') printf("\\t");
                else if (tk->text[i] == '\r') printf("\\r");
                else if (tk->text[i] == '\\') printf("\\\\");
                else printf("%c", tk->text[i]);
            }
        } else if (tk->code == CT_INT) {
            printf(" : %ld", tk->i);
        } else if (tk->code == CT_CHAR) {
            // Afiseaza caracterul escape intr-un mod lizibil daca este invizibil
            if (tk->i == '\n') printf(" : \\n");
            else if (tk->i == '\t') printf(" : \\t");
            else if (tk->i == '\0') printf(" : \\0");
            else if (tk->i == '\r') printf(" : \\r");
            else printf(" : %c", (char)tk->i);
        } else if (tk->code == CT_REAL) {
            printf(" : %f", tk->r);
        }
        
        printf("\n");
        tk = tk->next;
    }
}