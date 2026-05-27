#include <stdio.h>
#include "../header/semantic_domain_w_vector.h"
#include "../header/syntactical.h"
#include "../header/lexer.h"

Token *ConsumedTk;
Token *crtTk=NULL;

Symbols symbols;
Symbol *owner=NULL;

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
        Token *tkName=crtTk;
        if(consume(ID)){
            if(consume(LACC)){
                Symbol *s=findSymbolInDomain(&symbols,tkName->text);
                if(s){
                    tkerr(crtTk,"Semantic error : symbol redefinition: %s",tkName->text);
                }
                s=addSymbolToDomain(&symbols,tkName->text,CLS_STRUCT);
                s->type.typeBase=TB_STRUCT;
                s->type.s=s;
                s->type.nElements=-1;
                pushDomain();
                owner=s;
                while(1){
                    if(varDef()){}
                    else break;
                }
                if(consume(RACC)){
                    if(consume(SEMICOLON)){
                        owner=NULL;
                        dropDomain(&symbols);
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
    Type t;
    if(typeBase(&t)){
        Token *tkName=crtTk;
        if(consume(ID)){
            arrayDecl(&t);
            if(t.nElements==0)tkerr(crtTk,"Semantic error : a vector variable must have a specified dimension");
            if(consume(SEMICOLON)){
                Symbol *var=findSymbolInDomain(&symbols,tkName->text);
                if(var){
                    tkerr(crtTk,"Semantic error : symbol redefinition: %s",tkName->text);
                }
                var=addSymbolToDomain(&symbols,tkName->text,CLS_VAR);
                var->type=t;
                var->owner=owner;
                if(owner){
                    switch(owner->cls){
                        case CLS_FUNC:
                            
                    }
                }
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

int typeBase(Type *t){
    t->nElements=-1;
    if(consume(INT)){t->typeBase=TB_INT; return 1;}
    else if(consume(DOUBLE)){t->typeBase=TB_DOUBLE; return 1;}
    else if(consume(CHAR)){t->typeBase=TB_CHAR; return 1;}
    else if(consume(FLOAT)){t->typeBase=TB_FLOAT; return 1;} //becasue i added float token in lexical analyzer
    else if(consume(STRUCT)){
        Token *tkName=crtTk;
        if(consume(ID)){
            t->typeBase=TB_STRUCT;
            t->s=findSymbol(&symbols,tkName->text);
            if(!t->s){
                tkerr(crtTk,"Semantic error : undefined structure: %s",tkName->text);
            }
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing declaration");

        }
    }
    return 0;
}

int arrayDecl(Type *t){
    
    if(consume(LBRACKET)){
        Token *tkSize=crtTk;
        if(consume(CT_INT)){
            t->nElements=tkSize->i;
        }
        else{
            t->nElements=0;
        }
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
    Type t;
    if(typeBase(&t) || consume(VOID)){
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
    Type t;
    if(typeBase(&t)){
        if(consume(ID)){
            arrayDecl(&t);
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
    if(exprAssign()){
        return 1;
    }
    return 0;
}

int exprAssign(){

    Token *startTk=crtTk;
    if(exprUnary()){
        if(consume(ASSIGN)){
            if(exprAssign()){
                return 1;
            }
            else{
                tkerr(crtTk,"Syntax error : missing expression");
            }
        }
        else{
            crtTk=startTk;
            //return 0; can t return 0 because we need to try exprOr also in this function
        }
    }
    if(exprOr()){
        return 1;
    }
    return 0;
}

/*exprOr: exprOr OR exprAnd | exprAnd
exprOr=exprAnd exprOrAux
exprOrAux=OR exprAnd exprOrAux | epsilon
*/
int exprOr(){
    if(exprAnd()){
        if(exprOrAux()){
            return 1;
        }
        //we don t need else case because exprOrAux will always return 1 or will be a tkerr;
    }
    return 0;
}

int exprOrAux(){
    if(consume(OR)){
        if(exprAnd()){
            if(exprOrAux()){
                return 1;
            } //if we look at the example a || b and follow through the funtions at the end exprOrAux will return 1 and the syntactic is good
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1; //epsilon represents the empty string and lets the funtions to return true if it s nothing next
}

int exprAnd(){
    if(exprEq()){
        if(exprAndAux()){
            return 1;
        }
    }
    return 0;
}

int exprAndAux(){
    if(consume(AND)){
        if(exprEq()){
            if(exprAndAux()){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1; 
}

int exprEq(){
    if(exprRel()){
        if(exprEqAux()){
            return 1;
        }
    }
    return 0;
}

int exprEqAux(){
    if(consume(EQUAL) || consume(NOTEQ)){
        if(exprRel()){
            if(exprEqAux()){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

int exprRel(){
    if(exprAdd()){
        if(exprRelAux()){
            return 1;
        }
    }
    return 0;
}

int exprRelAux(){
    if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ)){
        if(exprAdd()){
            if(exprRelAux()){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

int exprAdd(){
    if(exprMul()){
        if(exprAddAux()){
            return 1;
        }
    }
    return 0;
}

int exprAddAux(){
    if(consume(ADD) || consume(SUB)){
        if(exprMul()){
            if(exprAddAux()){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

int exprMul(){
    if(exprCast()){
        if(exprMulAux()){
            return 1;
        }
    }
    return 0;
}

int exprMulAux(){
    if(consume(MUL) || consume(DIV)){
        if(exprCast()){
            if(exprMulAux()){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

int exprCast(){

    Token *startTk=crtTk;
    if(consume(LPAR)){
        Type t;
        if(typeBase(&t)){
            arrayDecl(&t);
            if(consume(RPAR)){
                if(exprCast()){
                    return 1;
                }
                else{
                    tkerr(crtTk,"Syntax error : missing expression");
                }
            }
            else{
                tkerr(crtTk,"Syntax error : missing )");
            }
        }
        crtTk=startTk; //if it s not a cast it s an exprUnary and we need to try it 
    }
    if(exprUnary()){
        return 1;
    }
    return 0;
}

int exprUnary(){
    if(consume(SUB) || consume(NOT)){
        if(exprUnary()){
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    if(exprPostfix()){
        return 1;
    }
    return 0;
}

/*
    exprPostfix: exprPostfix LBRACKET expr RBRACKET
    | exprPostfix DOT ID
    | exprPrimary
    A=Aalpha1|Aalpha2|beta1
*/
int exprPostfix(){
    if(exprPrimary()){
        if(exprPostfixAux()){
            return 1;
        }
    }
    return 0;
}

int exprPostfixAux(){
    if(consume(LBRACKET)){
        if(expr()){
            if(consume(RBRACKET)){
                if(exprPostfixAux()){
                    return 1;
                }
            }
            else{
                tkerr(crtTk,"Syntax error : missing ]");
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expr");
        }
    }
    if(consume(DOT)){
        if(consume(ID)){
            if(exprPostfixAux()){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing id");
        }
    }
    return 1;
}

int exprPrimary(){
    if(consume(ID)){
        if(consume(LPAR)){
            if(expr()){
                while(1){
                    if(consume(COMMA)){
                        if(expr()){
                            
                        }
                        else{
                            tkerr(crtTk,"Syntax error : missing expr");
                        }
                    }
                    else{
                        break;
                    }
                }
            }
            if(consume(RPAR)){
                    return 1;
            }
            else{
                tkerr(crtTk,"Syntax error : missing )");
            }
        }
        return 1;
    }
    if(consume(CT_CHAR)){
        return 1;
    }
    if(consume(CT_INT)){
        return 1;
    }
    if(consume(CT_REAL)){
        return 1;
    }
    if(consume(CT_STRING)){
        return 1;
    }
    if(consume(LPAR)){
        if(expr()){
            if(consume(RPAR)){
                return 1;
            } 
            else 
            {
                tkerr(crtTk,"Syntax error : missing )");
            }
        } 
        else 
        {
            tkerr(crtTk,"Syntax error : missing expr");
        }
    }
    return 0;
}
