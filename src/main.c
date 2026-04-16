#include <stdio.h>
#include <stdlib.h>
#include "../header/lexer.h"

// Functie helper pentru a pune tot fisierul in memorie
char* loadFile(const char* filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        exit(-1);
    }
    
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    if (!string) {
        fprintf(stderr, "Memory error\n");
        exit(-1);
    }
    
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = '\0'; // Adaugam terminatorul cerut de logica noastra
    return string;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Utilizare: %s <fisier_intrare.c>\n", argv[0]);
        return -1;
    }

    char *sourceCode = loadFile(argv[1]);
    pCrtCh = sourceCode;

    // Generam toti tokenii pana la final [cite: 74]
    while (getNextToken() != END) {}

    // Ii afisam in consola
    showTokens();

    free(sourceCode);
    return 0;

    
}