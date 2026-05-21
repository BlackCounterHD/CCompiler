#ifndef UTILS_H
#define UTILS_h

char *loadFile(const char *filename);
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

#endif