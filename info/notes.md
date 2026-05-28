

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

1. Global variables are created before runtime so if i try to assign the global variable crtTk in syntactical.c to head that will point to the head of the token list at **runtime** it will give an erro that s why i assign it in the **unit()** function.

## Syntactical Part

1. When startTk is needed : 
    1. At structDef: STRUCT ID LACC we can t use tkerr and stop the program if LACC is missing because we can have STRUCT ID ID ( struct Car car ) at varDef: typeBase ID where typeBase can be STRUCT ID
    2. At varDef: typeBase ID arrayDecl? SEMICOLON we can t use tkerr and stop the program if SEMICOLON is missing because we can have typeBase ID  LPAR at fnDef ( int a() )

## Semantical Part

1. The implementation for the domain analysis related to semantical uses an Array that will auto - increment itself when there is no room left for adding new Symbols ( symbols->end==symbols->after )

2. A second type of implementation could use linked list insead of a vector but , although it s easier to implement the add , delete and find functions the problem relies in **memory allocation** . When using an **array** the memory is a contignous block and all the pointers to the sysmbols are being put to addresses one near eachother and it s easier for the processor to read them using the cache memory . When using a linked list the memory is spread trough the RAM and the addreses are random resulting in a harder iteration for the processor .

3. A third type of implementation that i could **try** is  using a hash table that most compilers use.

4. **s** parameter in type and **owner**
    ```c
        struct Patrat {
            int x; // owner = Patrat
        };

        struct Triunghi {
            int x; // owner = Triunghi
        };

        struct Patrat p1;   // type.s = Patrat
        struct Triunghi t1; // type.s = Triunghi
    ```
5. **inout and out** . If is **out** it means it s modified in that function , if **inout** it means it comes with something and also is modified in that function

6. **varIdx and paramIdx** represent the memory location of the variable or func parameter in the memory to be later used at code generation (for a virtual machine or assembly code) . our compiler will only need the memory locations not the names.

7. 
   1. Function parameter (SK_PARAM ): Will be initialized with cls = CLS_VAR and mem = MEM_ARG. 
   2. Local variable (SK_VAR inside a function ): Will be initialized with cls = CLS_VAR and mem = MEM_LOCAL. 
   3. Global variable (SK_VAR outside functions ): Will be initialized with cls = CLS_VAR and mem = MEM_GLOBAL

8. the variable **newDomain** is used for marking a new domain for if s while s for s etc..

## Pointers

1. From :
    ```c
        void showTokens();
        void showTokensInFile();
        void freeTokenList();
    ```
i took out the argument Token *head from those functions because it s not needed . We work with the global variable head 

2. In functions that we pass symbols as a reference we need to acces the values and we would have done for example (*symbols).begin=... but we can do it more simple like : symbols->begin

3. Why do we need to use ** for the vector of pointers (begin,end,after) ?
    1. Because we work with a vector of pointers of type Symbol . if we didnt use ** we would have needed to work with a vector of symbols which would have been much bigger in terms of memory  we couldn t use SAFEALLOC because it returns a pointer to an address and we work with a vector of symbols.
    2. If we have had only  the vector of symsbols and we couldn t use safealloc whenever we used in a function ex.type.s=s (when we reference a struct variable to a struct or smth similar) it would crash at the next realloc because that address is lost (becasue of realocating the big vector of symbols) . Instead, if we use a vector of pointers whenever we create a new Symbol with safealloc and add it to the vector that address won t be lost until we free it.
    3. 
     ```c
        int x=5;
        int *y=x; F
        int *y=&x; T

        symbols->end=s; F
        symbols->end=&s; T
        *symbols->end=s; T
    ```