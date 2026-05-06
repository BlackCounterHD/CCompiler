

# Notes

## Left Recursion needs to be Eliminated !

1. exprOr: exprOr OR exprAnd | exprAnd here when exprOr is called it will enter in an infinite recursion because it will call itself and crtTk will never advance that s why we use the formula rpesented in resources/syntacticalRecRule.jpg
2. exprAssign: exprUnary ASSIGN exprAssign | exprOr here , when il will call exprAssign again crtTk is already advanced ( ex : x=y=5 exprUnary takes x , assign takes first = then it calls exprAssign and again exprUnary takes y then Assign takes second = then it calls exprAssign again and takes the final 5)

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
4. **void tkerr(const Token *tk,const char *fmt,...)** this is a variadic function like printf fprintf etc.  and it accepts as many arguments as you want due to ... like printf (man printf)

```c

void tkerr(const Token *tk,const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(1);
}

```
vfprintf is for variadic funtions that s why it starts whith v at it concatenates everything to what we already printed here : fprintf(stderr,"error in line %d: ",tk->line);

## Errors

I added tkerr in the states where is no coming back if i introduce a bad token for example 1.Y34 will go to error but 1.34Yb123 1.34 will be CT_REAL Yb123 will be an ID and this will be a problem for the syntactical analyzer to solve.

## Difference beetween AtomC and standar C

1. In C is valid **123.** or **.097** but in AtomC is not valid . If i lll decide to implement i need to change in state 43 and state 10 !
2. I can also modify it to support **linecomments** /*  *\
3. Also base 2 numbers plus bitwise operators : & , | , ~ , ^ 

## Global variables

1. Global variables are created before runtime so if i try to assign the global variable crtTk in syntactical.c to head that will point to the head of the token list at **runtime** it will give an erro that s why i assign it in the **parse()** function.

## Syntactical Part

1. When startTk is needed : 
    1. At structDef: STRUCT ID LACC we can t use tkerr and stop the program if LACC is missing because we can have STRUCT ID ID ( struct Car car ) at varDef: typeBase ID where typeBase can be STRUCT ID
    2. At varDef: typeBase ID arrayDecl? SEMICOLON we can t use tkerr and stop the program if SEMICOLON is missing because we can have typeBase ID  LPAR at fnDef ( int a() )

