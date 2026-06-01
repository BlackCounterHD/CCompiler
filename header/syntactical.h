#ifndef SYNTACTICAL_H
#define SYNTACTICAL_H

#include "../header/semantic_domain_w_vector.h"
#include "../header/semantic_type.h"
#include "../header/lexer.h"

extern Token *crtTk;

int consume(int code);
int unit();
int structDef();
int varDef();
int typeBase(Type *t);
int arrayDecl(Type *t);
int fnDef();
int fnParam();
int stm();
int stmcompound(int newDomain);
int expr(RetVal *r);
int exprAssign(RetVal *r);
int exprOr(RetVal *r);
int exprOrAux(RetVal *r);
int exprAnd(RetVal *r);
int exprAndAux(RetVal *r);
int exprEq(RetVal *r);
int exprEqAux(RetVal *r);
int exprRel(RetVal *r);
int exprRelAux(RetVal *r);
int exprAdd(RetVal *r);
int exprAddAux(RetVal *r);
int exprMul(RetVal *r);
int exprMulAux(RetVal *r);
int exprCast(RetVal *r);
int exprUnary(RetVal *r);
int exprPostfix(RetVal *r);
int exprPostfixAux(RetVal *r);
int exprPrimary(RetVal *r);

#endif