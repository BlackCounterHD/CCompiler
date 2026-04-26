

#include <stdio.h>
#include <stdlib.h>
#include "../header/utils.h"

char *loadFile(const char *filename){
    FILE *f=fopen(filename,"rb");

    if(!f){
        fprintf(stderr,"Error at opening the file : %s\n",filename);
        return NULL;
    }

    fseek(f,0,SEEK_END);
    
    long fsize = ftell(f);

    fseek(f,0,SEEK_SET);

    char *bigString = malloc(fsize+1);

    if(!bigString){
        fprintf(stderr,"Eroor at allocating memory for bigString\n");
        return NULL;
    }

    fread(bigString,fsize,1,f);
    
    fclose(f);

    bigString[fsize]='\0';
    
    return bigString;

}
