

# Notes

## Extern variables

1. I can declare them in a **header file** but i need to include that header file whenever i wanna use them
2. I must declare them in a C file (globally) to allocate the block of memory for them
3. Whenever i use them in another files of other functions i will always work with that block of memory and with that variable by just using it s name
4. I can t declare it in multiple files because that ll give a multiple declaration error but if i declare in a function a variable with that name it will be local to that function and be a different variable with a different block of memory

## Const Variables

1. Functions like **memcmp** use const variables 
2. A variable that is read only and can t be modified 

## Functions

1. Functions like **memcmp , fseek , ftell** very good documentation on **man**
2. in **enum** are the codes for ID's and keywords . In a token we assign the code (which is an int) to a name in enum like tk->code=WHILE . **enum** s a way to create my own custom data type consisting of a list of named integer constants. With the help of enum i declared const char *tokenNames[] and i always can print the code like this : found %s\n",curr->line,tokenNames{curr->code} but the order in enum needs to be exactly the same as in tokenNames

```c
enum Tokens {
    ID,      // The compiler sees this as 0
    BREAK,   // The compiler sees this as 1
    CHAR,    // The compiler sees this as 2
    FLOAT    // The compiler sees this as 3
    .
    .
    .
};
```
3. fseek is like a cursour on a monitor . It points to where we need in the file : **fseek(f,0,SEEK_END)** will point to the end of the file then we store in a **long x** variable the value that **ftell** will give us (that will be the size of the file) . Then we set the pointer to the beginning of the file because we ll need to read the data from the file and we need to read from the beginning. 

```c
fseek(f, 0, SEEK_END);
    
long fsize = ftell(f);

fseek(f, 0, SEEK_SET);

char *bigString = malloc(fsize + 1);

if(!bigString){
    fprintf(stderr, "Error allocating memory for bigString\n");
    return NULL;
}


fread(bigString, fsize, 1, f);
```

## Errors

I added tkerr in the states where is no coming back if i introduce a bad token for example 1.Y34 will go to error but 1.34Yb123 1.34 will be CT_REAL Yb123 will be an ID and this will be a problem for the syntactical analyzer to solve.

## Difference beetween AtomC and standar C

1. In C is valid **123.** or **.097** but in AtomC is not valid . If i lll decide to implement i need to change in state 43 and state 10 !
2. I can also modify it to support **linecomments** /*  *\
3. Also base 2 numbers plus bitwise operators : & , | , ~ , ^ 