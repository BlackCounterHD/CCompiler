#ifndef SYNTACTICAL_H
#define SYNTACTICAL_H


int consume(int code);
int unit();
int structDef();
int varDef();
int typeBase(Type *t);
int arrayDecl(Type *t);
int fnDef();
int fnParam();
int stm();
int stmcompound();
int expr();
int exprAssign();
int exprOr();
int exprOrAux();
int exprAnd();
int exprAndAux();
int exprEq();
int exprEqAux();
int exprRel();
int exprRelAux();
int exprAdd();
int exprAddAux();
int exprMul();
int exprMulAux();
int exprCast();
int exprUnary();
int exprPostfix();
int exprPostfixAux();
int exprPrimary();

#endif