#ifndef UTILS_H
#define UTILS_H

#include "../header/lexer.h" //for err function

char *loadFile(const char *filename);
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

#endif