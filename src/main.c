
#include <stdio.h>
#include <stdlib.h>
#include "../header/lexer.h"
#include "../header/utils.h"
#include "../header/syntactical.h"

int main(int argc,char **argv){

    char fileName[256];
    char *filePointer=NULL;
    
    if(argc>1){
        filePointer=loadFile(argv[1]);
    }

    while(filePointer==NULL){
        fprintf(stderr,"Please input a valid file name : ");

        scanf("%255s",fileName);

        filePointer=loadFile(fileName);
    }

    pCrtCh=filePointer;

    while(getNextToken()!=END){}

    showTokensInFile(head);

    unit();

    freeTokenList(head);

    return 0;
    
}