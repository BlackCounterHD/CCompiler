#include <stdio.h>
#include "../header/syntactical.h"
#include "../header/lexer.h"

Token *ConsumedTk;
Token *crtTk=NULL;

int consume(int code){
    if(crtTk->code==code){
        ConsumedTk=crtTk;
        crtTk=crtTk->next;
        return 1;
    }
    return 0;
}

int unit(){

    crtTk=head;

    while(1)
    {
        if(structDef()){}
        else if(varDef()){} //testing for vadDef first
        else if(fnDef()){}
        else break;
    }
    if(consume(END)){
        return 1;
    }
    else{
        tkerr(crtTk,"Syntax error : Invalid code at global level");
    }
    return 0;

}


int structDef(){

    Token *startTk=crtTk;
    if(consume(STRUCT)){
        if(consume(ID)){
            if(consume(LACC)){
                while(1){
                    if(varDef()){}
                    else break;
                }
                if(consume(RACC)){
                    if(consume(SEMICOLON)){
                        return 1;
                    }
                    else{
                        tkerr(crtTk,"Syntax error : missing ;");
                    }
                }
                else{
                    tkerr(crtTk,"Syntax error : missing }");
                }
            }
            else{
                //startTk is used because we can have struct ID ID  ex : struct Car car at varDef
                //that s why we can t use tkerr and stop the program
                crtTk=startTk; 
                return 0;
            }
        }
        else {
            tkerr(crtTk,"Syntax error : missing declaration");
        }
    }
    return 0;
    
}

int varDef(){

    Token *startTk=crtTk;
    if(typeBase()){
        if(consume(ID)){
            arrayDecl();
            if(consume(SEMICOLON)){
                return 1;
            }
            else {
                //can t use tkerr because at fnDef we can have typeBase ID LPAR etc..
                crtTk=startTk;
                return 0;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing declaration");
        }
    }
    return 0;
    
}

int typeBase(){
    if(consume(INT)){return 1;}
    else if(consume(DOUBLE)){return 1;}
    else if(consume(CHAR)){return 1;}
    else if(consume(FLOAT)){return 1;} //becasue i added float token in lexical analyzer
    else if(consume(STRUCT)){
        if(consume(ID)){
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing declaration");

        }
    }
    return 0;
}

int arrayDecl(){
    
    if(consume(LBRACKET)){
        consume(CT_INT);
        if(consume(RBRACKET)){
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing ]");
        }
    }
    return 0;
    
}

int fnDef(){
    
    if(typeBase() || consume(VOID)){
        if(consume(ID)){
            if(consume(LPAR)){
                if(fnParam()){
                    while(1){
                        if(consume(COMMA)){
                            if(fnParam()){}
                            else{
                                tkerr(crtTk,"Syntax error : missing parameter after ,");
                            }
                        }
                        else{
                            break;
                        }
                    }
                }
                if(consume(RPAR)){
                    if(stmcompound()){
                        return 1;
                    }
                    else {
                        tkerr(crtTk, "Syntax error : missing function body");
                    }
                }
                else{
                    tkerr(crtTk,"Syntax error : missing )");
                }
            }
            else{
                tkerr(crtTk,"Syntax error : missing (");
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing declaration");
        }
    }
    return 0;
    
}

int fnParam() {
    if(typeBase()){
        if(consume(ID)){
            arrayDecl();
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing function parameter");
        }
    }
    return 0; 
}

int stm(){

    if(stmcompound()){
        return 1;
    }

    if(consume(IF)){
        if(consume(LPAR)){
            if(expr()){
                if(consume(RPAR)){
                    if(stm()){
                        if(consume(ELSE)){
                            if(stm()){
                                return 1;
                            }
                            else{
                                tkerr(crtTk,"Syntax error : missing else statement");
                            }
                        }
                        return 1;
                    }
                    else{
                        tkerr(crtTk,"Syntax error : missing if statement");
                    }
                }
                else{
                    tkerr(crtTk,"Syntax error : missing )");
                }
            }
            else{
                tkerr(crtTk,"Syntax error : missing if expression");
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing (");
        }
    }

    if(consume(WHILE)){
        if(consume(LPAR)){
            if(expr()){
                if(consume(RPAR)){
                    if(stm()){
                        return 1;
                    }
                    else{
                        tkerr(crtTk,"Syntax error : missing while statement");
                    }
                }
                else{
                    tkerr(crtTk,"Syntax error : missing )");
                }
            }
            else{
                tkerr(crtTk,"Syntax error : missing while expression");
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing (");
        }
    }
    
    if(consume(FOR)){
        if(consume(LPAR)){
            expr();   
            if(consume(SEMICOLON)){
                expr();
                if(consume(SEMICOLON)){
                    expr();
                    if(consume(RPAR)){
                        if(stm()){
                            return 1;
                        }
                        else{
                            tkerr(crtTk,"Syntax error : missing for statement");
                        }
                    }
                    else{
                        tkerr(crtTk,"Syntax error : missing )");
                    }
                }
                else{
                    tkerr(crtTk,"Syntax error : missing second ; expression");
                }
            }
            else{
                tkerr(crtTk,"Syntax error : missing first ; expression");
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing (");
        }
    }

    if(consume(BREAK)){
        if(consume(SEMICOLON)){
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing ;");
        }
    }

    if(consume(RETURN)){
        expr();
        if(consume(SEMICOLON)){
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing ;");
        }
    }

    if (expr()) {
        if (consume(SEMICOLON)) return 1;
        else tkerr(crtTk, "Syntax error : missing ; ");
    }
    else if (consume(SEMICOLON)) {
        return 1; 
    }
    
    return 0;
}

int stmcompound() {

    if(consume(LACC)){
        while(1){
            if(varDef() || stm()){}
            else break;
        }
        if(consume(RACC)){
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing }");
        }
    }
    return 0;
}

int expr(){
    return 0;
}



