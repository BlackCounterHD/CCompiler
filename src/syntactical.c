#include <stdio.h>
#include "../header/semantic_type.h"
#include "../header/semantic_domain_w_vector.h"
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

// the structure name must be unique in the domain 
// inside the structure, there cannot be two variables with the same name 
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

// the variable name must be unique in the domain 
// array variables must have a specified dimension (not accepted: int v[]) 
int varDef(){

    Token *startTk=crtTk;
    Type t;
    if(typeBase(&t)){
        Token *tkName=crtTk;
        if(consume(ID)){
            if(arrayDecl(&t)){
                if(t.nElements==0)tkerr(crtTk,"Semantic error : a vector variable must have a specified dimension");
            }
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
                            var->varIdx=symbolsLen(owner->fn.locals);
                            var->mem = MEM_LOCAL;
                            addSymbolToList(&owner->fn.locals,dupSymbol(var));
                            break;
                        case CLS_STRUCT:
                            var->varIdx=typeSize(&owner->type);
                            addSymbolToList(&owner->members,dupSymbol(var));
                            break;
                    }
                }
                else{
                    var->varIdx=allocInGlobalMemory(typeSize(&t));
                    var->mem = MEM_GLOBAL;
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

// if the base type is a structure, it must be already defined beforehand 
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

// the function name must be unique in the domain 
// the function's local domain starts immediately after LPAR 
// the function body {...} does not define a new subdomain inside the function's local domain 
int fnDef(){
    Type t;
    int hasType = typeBase(&t);
    //here hastype has the job of a flag it will be one if the function type is int,float,double,char
    //if it s not one and the function is void it s will be set to one and continue
    if(!hasType && consume(VOID)){
        t.typeBase = TB_VOID;
        t.nElements=-1;
        hasType = 1; 
    }
    if(hasType){
        Token *tkName=crtTk;
        if(consume(ID)){
            if(consume(LPAR)){
                Symbol *fn=findSymbolInDomain(&symbols,tkName->text);
                if(fn)tkerr(crtTk,"symbol redefinition: %s",tkName->text);
                fn=addSymbolToDomain(&symbols,tkName->text,CLS_FUNC);
                fn->type=t;
                owner=fn;
                pushDomain();
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
                    if(stmcompound(0)){ //0 because the domain was already opened above when declaring the function parameters and now when declaring variables or anything else we are in same domain
                        dropDomain(&symbols);
                        owner=NULL;
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

// the parameter name must be unique in the domain 
// parameters can be arrays with a given dimension, but in this case their dimension is erased (int v[10] -> int v[]) 
// the parameter is added to both the current domain and the function's parameters list 
int fnParam() {
    Type t;
    if(typeBase(&t)){
        Token *tkName=crtTk;
        if(consume(ID)){
            if(arrayDecl(&t)){
                t.nElements=0;
            }
            Symbol *param=findSymbolInDomain(&symbols,tkName->text);
            if(param)tkerr(crtTk,"symbol redefinition: %s",tkName->text);
            param=addSymbolToDomain(&symbols,tkName->text,CLS_VAR);
            param->type=t;
            param->mem = MEM_ARG;
            param->paramIdx=symbolsLen(owner->fn.params);
            // the parameter is added to both the current domain and the fn parameters
            addSymbolToList(&owner->fn.params,dupSymbol(param));
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing function parameter");
        }
    }
    return 0; 
}

// the compound body {...} of the instructions defines a new domain 

// the condition for 'if', 'while', or 'for' must evaluate to a scalar value
// a void function cannot return a value
// the returned expression must evaluate to a scalar value
// the return value must be convertible to the function's return type
// a non-void function must return a value
int stm(){

    RetVal rInit; //ex : i=0;
    RetVal rCond; //ex : a<10
    RetVal rStep; //ex : i++
    RetVal rExpr; //ex : return x
    //those stock the result of different expressions
    if(stmcompound(1)){
        return 1;
    }

    if(consume(IF)){
        if(consume(LPAR)){
            if(expr(&rCond)){
                {if(!canBeScalar(&rCond))tkerr(crtTk,"the if condition must be a scalar value");}//scalar value is holding a single value! (not vectors,structs etc..)
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
            if(expr(&rCond)){
                {if(!canBeScalar(&rCond))tkerr(crtTk,"the while condition must be a scalar value");}
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
            expr(&rInit);   
            if(consume(SEMICOLON)){
                if(expr(&rCond)){
                    if(!canBeScalar(&rCond))tkerr(crtTk,"the for condition must be a scalar value");
                }
                if(consume(SEMICOLON)){
                    expr(&rStep);
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
        if(expr(&rExpr)){
        if(owner->type.typeBase==TB_VOID)tkerr(crtTk,"a void function cannot return a value");
        if(!canBeScalar(&rExpr))tkerr(crtTk,"the return value must be a scalar value");
        if(!convTo(&rExpr.type,&owner->type))tkerr(crtTk,"cannot convert the return expression type to the function return type");
        }
        else {
            if(owner->type.typeBase!=TB_VOID)tkerr(crtTk,"a non-void function must return a value");
        }
        if(consume(SEMICOLON)){
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing ;");
        }
    }

    if (expr(&rExpr)) {
        if (consume(SEMICOLON)) return 1;
        else tkerr(crtTk, "Syntax error : missing ; ");
    }
    else if (consume(SEMICOLON)) {
        return 1; 
    }
    
    return 0;
}

// a new domain is defined only on request (newDomain)
int stmcompound(int newDomain) {

    //here newDomain is helpfull when we declare a function we create a new domain add it s parameters 
    //then in the same domain we add the local variables and in case of if s for s etc.. we create new domains
    //follow the code from fnDef ... consume(RPAR)
    if(consume(LACC)){
        if(newDomain) pushDomain();
        while(1){
            if(varDef() || stm()){}
            else break;
        }
        if(consume(RACC)){
            if(newDomain) dropDomain(&symbols);
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing }");
        }
    }
    return 0;
}

int expr(RetVal *r){
    if(exprAssign(r)){
        return 1;
    }
    return 0;
}

// the assign destination (left side) must be a left-value (memory address)
// the assign destination cannot be a constant (e.g., an array name)
// both the destination and the source must be scalar values
// the assign source must be convertible to the destination's type
// the result of an assignment becomes a constant right-value
int exprAssign(RetVal *r){

    Token *startTk=crtTk;
    RetVal rDst;
    if(exprUnary(&rDst)){
        if(consume(ASSIGN)){
            if(exprAssign(r)){
                if(!rDst.isLVal)tkerr(crtTk,"the assign destination must be a left-value");
                if(rDst.isCtVal)tkerr(crtTk,"the assign destination cannot be constant");
                if(!canBeScalar(&rDst))tkerr(crtTk,"the assign destination must be scalar");
                if(!canBeScalar(r))tkerr(crtTk,"the assign source must be scalar");
                if(!convTo(&r->type,&rDst.type))tkerr(crtTk,"the assign source cannot be converted to destination");
                r->isLVal=0;
                r->isCtVal=1;
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
    if(exprOr(r)){
        return 1;
    }
    return 0;
}

/*exprOr: exprOr OR exprAnd | exprAnd
exprOr=exprAnd exprOrAux
exprOrAux=OR exprAnd exprOrAux | epsilon
*/

// both operands must be scalar values compatible with logical operations
// the result of a logical operation is always a boolean (represented as TB_INT), r-value, constant
int exprOr(RetVal *r){
    if(exprAnd(r)){
        if(exprOrAux(r)){
            return 1;
        }
        //we don t need else case because exprOrAux will always return 1 or will be a tkerr;
    }
    return 0;
}

int exprOrAux(RetVal *r){
    if(consume(OR)){
        RetVal right;
        if(exprAnd(&right)){
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(crtTk,"invalid operand type for ||");

            r->type.typeBase = TB_INT;
            r->type.s = NULL;
            r->type.nElements = -1;
            r->isLVal = 0;
            r->isCtVal = 1;
            
            if(exprOrAux(r)){
                return 1;
            } //if we look at the example a || b and follow through the funtions at the end exprOrAux will return 1 and the syntactic is good
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1; //epsilon represents the empty string and lets the funtions to return true if it s nothing next
}

// both operands must be scalar values compatible with logical operations
// the result of a logical operation is always a boolean (represented as TB_INT), r-value, constant
int exprAnd(RetVal *r){
    if(exprEq(r)){
        if(exprAndAux(r)){
            return 1;
        }
    }
    return 0;
}

int exprAndAux(RetVal *r){
    if(consume(AND)){
        RetVal right;
        if(exprEq(&right)){
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(crtTk,"invalid operand type for &&");

            r->type.typeBase = TB_INT;
            r->type.s = NULL;
            r->type.nElements = -1;
            r->isLVal = 0;
            r->isCtVal = 1;
            if(exprAndAux(r)){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1; 
}

// both operands must be compatible scalar types
// the result of a comparison is always a boolean (represented as TB_INT), r-value, constant
int exprEq(RetVal *r){
    if(exprRel(r)){
        if(exprEqAux(r)){
            return 1;
        }
    }
    return 0;
}

int exprEqAux(RetVal *r){
    if(consume(EQUAL) || consume(NOTEQ)){
        RetVal right;
        if(exprRel(&right)){
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(crtTk,"invalid operand type for == or !=");

            r->type.typeBase = TB_INT;
            r->type.s = NULL;
            r->type.nElements = -1;
            r->isLVal = 0;
            r->isCtVal = 1;
            if(exprEqAux(r)){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

// both operands must be compatible scalar types
// the result of a comparison is always a boolean (represented as TB_INT), r-value, constant
int exprRel(RetVal *r){
    if(exprAdd(r)){
        if(exprRelAux(r)){
            return 1;
        }
    }
    return 0;
}

int exprRelAux(RetVal *r){
    if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ)){
        RetVal right;
        if(exprAdd(&right)){
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(crtTk,"invalid operand type for <,<=,>,>=");

            r->type.typeBase = TB_INT;
            r->type.s = NULL;
            r->type.nElements = -1;
            r->isLVal = 0;
            r->isCtVal = 1;
            //here and in the above logical extressions
            //the result is always a boolean (0 or 1)
            //that s why r->type.typeBase = TB_INT; r->type.s = NULL; r->type.nElements = -1;
            if(exprRelAux(r)){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

// both operands must be scalar types
// the result type is calculated by promoting the operands (arithTypeTo)
// the result becomes a right-value and a constant
int exprAdd(RetVal *r){
    if(exprMul(r)){
        if(exprAddAux(r)){
            return 1;
        }
    }
    return 0;
}

int exprAddAux(RetVal *r){
    if(consume(ADD) || consume(SUB)){
        RetVal right;
        if(exprMul(&right)){
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(crtTk,"invalid operand type for + or -");

            r->type = tDst;
            r->isLVal = 0;
            r->isCtVal = 1;
            //here and in the next arithmetic expressions we need the result
            //that is combined in tDst via arithTypeTo
            if(exprAddAux(r)){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

// both operands must be scalar types
// the result type is calculated by promoting the operands (arithTypeTo)
// the result becomes a right-value and a constant
int exprMul(RetVal *r){
    if(exprCast(r)){
        if(exprMulAux(r)){
            return 1;
        }
    }
    return 0;
}

int exprMulAux(RetVal *r){
    if(consume(MUL) || consume(DIV)){
        RetVal right;
        if(exprCast(&right)){
            Type tDst;
            if(!arithTypeTo(&r->type,&right.type,&tDst))tkerr(crtTk,"invalid operand type for + or -");

            r->type = tDst;
            r->isLVal = 0;
            r->isCtVal = 1;
            if(exprMulAux(r)){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    return 1;
}

// cannot cast to a struct type
// cannot cast a struct variable to another type
// an array can only be cast to another array
// a scalar can only be cast to another scalar
int exprCast(RetVal *r){

    Token *startTk=crtTk;
    if(consume(LPAR)){
        Type t;
        RetVal op;
        if(typeBase(&t)){
            arrayDecl(&t);
            if(consume(RPAR)){
                if(exprCast(&op)){
                    if(t.typeBase==TB_STRUCT)tkerr(crtTk,"cannot convert to a struct type");
                    if(op.type.typeBase==TB_STRUCT)tkerr(crtTk,"cannot convert a struct");
                    if(op.type.nElements>=0 && t.nElements<0)tkerr(crtTk,"an array can be converted only to another array");
                    if(op.type.nElements<0 && t.nElements>=0)tkerr(crtTk,"a scalar can be converted only to another scalar");
                    r->type = t;
                    r->isLVal = 0;
                    r->isCtVal = 1;
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
    if(exprUnary(r)){
        return 1;
    }
    return 0;
}

// the unary operator requires a scalar operand
// the result becomes a right-value and a constant
int exprUnary(RetVal *r){
    if(consume(SUB) || consume(NOT)){
        if(exprUnary(r)){
            if(!canBeScalar(r))tkerr(crtTk,"unary - must have a scalar operand");
            r->isLVal=0;
            r->isCtVal=1;
            return 1;
        }
        else{
            tkerr(crtTk,"Syntax error : missing expression");
        }
    }
    if(exprPostfix(r)){
        return 1;
    }
    return 0;
}

/*
    exprPostfix: exprPostfix LBRACKET expr RBRACKET
    | exprPostfix DOT ID
    | exprPrimary
    A=Aalpha1|Aalpha2|beta1

    * A = beta A'
    * A' = alpha1 A' | alpha2 A' | epsilon
    
    exprPostfix = exprPrimary exprPostfixAux
    exprPostfixAux = LBRACKET expr RBRACKET exprPostfixAux 
    | DOT ID exprPostfixAux 
    | epsilon
*/

// only an array can be indexed
// the index expression must be convertible to an integer
// the result of indexing an array is a scalar left-value (not constant)

// a field can only be selected from a struct variable
// the specified field must exist in the structure's definition
// the result is a left-value. It is constant only if the field is an array.
int exprPostfix(RetVal *r){
    if(exprPrimary(r)){
        if(exprPostfixAux(r)){
            return 1;
        }
    }
    return 0;
}

int exprPostfixAux(RetVal *r){
    if(consume(LBRACKET)){
        RetVal idx;
        if(expr(&idx)){
            if(consume(RBRACKET)){
                if(r->type.nElements<0)tkerr(crtTk,"only an array can be indexed");
                Type tInt;
                tInt.typeBase=TB_INT;
                tInt.s=NULL;
                tInt.nElements=-1;
                if(!convTo(&idx.type,&tInt))tkerr(crtTk,"the index is not convertible to int");
                r->type.nElements=-1;
                r->isLVal=1;
                r->isCtVal=0;
                if(exprPostfixAux(r)){
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
        Token *tkName=crtTk;
        if(consume(ID)){
            if(r->type.typeBase!=TB_STRUCT)tkerr(crtTk,"a field can only be selected from a struct");
            Symbol *s=findSymbolInList(&r->type.s->members,tkName->text);
            if(!s)tkerr(crtTk,"the structure %s does not have a field %s",r->type.s->name,tkName->text);
            r->type=s->type;
            r->isLVal=1;
            r->isCtVal = (s->type.nElements >= 0) ? 1 : 0;
            if(exprPostfixAux(r)){
                return 1;
            }
        }
        else{
            tkerr(crtTk,"Syntax error : missing id");
        }
    }
    return 1;
}

// the identifier must be defined in the symbol table
// the identifier must represent a function (or external function) to be called
// the number of given arguments must not exceed the number of parameters
// the argument type must be convertible to the corresponding parameter type
// too few arguments: all parameters must be provided with arguments
// the result of a function call is a right-value, constant, with the function's return type
int exprPrimary(RetVal *r){
    Token *tkName=crtTk;
    if(consume(ID)){
        Symbol *s=findSymbol(&symbols,tkName->text);
        if(!s)tkerr(crtTk,"undefined id: %s",tkName->text);
        if(consume(LPAR)){
            if(s->cls != CLS_FUNC && s->cls != CLS_EXTFUNC)tkerr(crtTk,"only a function can be called");
            RetVal rArg;
            Symbol **crtparam=s->fn.params.begin;
            if(expr(&rArg)){
                if(crtparam == s->fn.params.end)tkerr(crtTk,"too many arguments in function call");
                if(!convTo(&rArg.type,&(*crtparam)->type))tkerr(crtTk,"in call, cannot convert the argument type to the parameter type");
                crtparam++;
                while(1){
                    if(consume(COMMA)){
                        if(expr(&rArg)){
                            if(crtparam == s->fn.params.end)tkerr(crtTk,"too many arguments in function call");
                            if(!convTo(&rArg.type,&(*crtparam)->type))tkerr(crtTk,"in call, cannot convert the argument type to the parameter type");
                            crtparam++;
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
                if(crtparam != s->fn.params.end)tkerr(crtTk,"too few arguments in function call");
                r->type=s->type;
                r->isLVal=0;
                r->isCtVal=1;
                return 1;
            }
            else{
                tkerr(crtTk,"Syntax error : missing )");
            }
        }
        // a function identifier cannot be used as a regular variable without being called
        // the result is a left-value. It is constant only if the variable is an array.
        else {
           
            if(s->cls == CLS_FUNC || s->cls == CLS_EXTFUNC) tkerr(crtTk, "a function can only be called");
            r->type = s->type;
            r->isLVal = 1;
            r->isCtVal = (s->type.nElements >= 0) ? 1 : 0;
            
        }
        return 1;
    }
    if(consume(CT_CHAR)){
        r->type.typeBase = TB_CHAR;
        r->type.s = NULL;
        r->type.nElements = -1;
        r->isLVal = 0;
        r->isCtVal = 1;
        return 1;
    }
    if(consume(CT_INT)){
        r->type.typeBase = TB_INT;
        r->type.s = NULL;
        r->type.nElements = -1;
        r->isLVal = 0;
        r->isCtVal = 1;
        return 1;
    }
    if(consume(CT_REAL)){
        r->type.typeBase = TB_DOUBLE;
        r->type.s = NULL;
        r->type.nElements = -1;
        r->isLVal = 0;
        r->isCtVal = 1;
        return 1;
    }
    if(consume(CT_STRING)){
        r->type.typeBase = TB_CHAR;
        r->type.s = NULL;
        r->type.nElements = 0;
        r->isLVal = 0;
        r->isCtVal = 1;
        return 1;
    }
    if(consume(LPAR)){
        if(expr(r)){
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
