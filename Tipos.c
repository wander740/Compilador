#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SYMTBL_SZ 26

char symtbl[SYMTBL_SZ]; /* tabela de símbolos */
char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */

void init();
void nextchar();
void error(char *s);
void fatal(char *s);
void expected(char *s);
void dumptable();
int isaddop(char c);
int ismulop(char c);
int isorop(char c);
int isrelop(char c);
void skipwhite();
void newline();
void match(char c);
char getname();
long getnum();

void addsymbol(char name, char type);
void checkdup(char name);
void duplicated(char name);
char intable(char name);
char symtype(char name);
void unrecognized(char name);
void asm_allocvar(char name, char type);
void decl();
void topdecls();
void asm_loadvar(char name, char type);
int isvartype(char c);
void notvar(char name);
char vartype(char name);
char loadvar(char name);
void asm_storevar(char name, char type);
void storevar(char name, char srctype);
char expression();
void assignment();
void block();
void asm_convert(char src, char dst);
void asm_loadconst(long val, char type);
char loadnum(long val);
char unop();
void asm_push(char type);
char add(char type);
char subtract(char type);
void asm_pop(char type);
void asm_swap(char type);
char asm_sametype(char t1, char t2, int ord_matters);
char asm_popadd(char t1, char t2);
char asm_popsub(char t1, char t2);
char factor();
char multiply(char type);
char divide(char type);
char term();
char asm_popmul(char t1, char t2);
char asm_popdiv(char t1, char t2);
void asm_clear();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    topdecls();
    //symtbl[0] = 'a';
    //symtbl[5] = 'p';
    //symtbl[10] = 'x';
    //addsymbol('A', 'a');
    //addsymbol('F', 'p');
    //addsymbol('K', 'x');
    //loadvar('A');
    //loadvar('B');
    //loadvar('C');
    //loadvar('X');
    match('B');
    newline();
    block();

    dumptable();

    return 0;

}

/* inicialização do compilador */
void init()
{
    int i;

    for (i = 0; i < SYMTBL_SZ; i++)
        symtbl[i] = '?';

    nextchar();
    skipwhite();

}

/* lê próximo caracter da entrada em lookahead */
void nextchar()
{
    look = getchar();

}

/* imprime mensagem de erro sem sair */
void error(char *s)
{
    fprintf(stderr, "Error: %s\n", s);

}

/* imprime mensagem de erro e sai */
void fatal(char *s)
{
    error(s);
    exit(1);

}

/* mensagem de erro para string esperada */
void expected(char *s)
{
    fprintf(stderr, "Error: %s expected\n", s);
    exit(1);

}

/* exibe a tabela de símbolos */
void dumptable()
{
    int i;
    printf("Symbol table dump:\n");

    for (i = 0; i < SYMTBL_SZ; i++)
        if (symtbl[i] != '?')
            printf("%c = %c\n", i + 'A', symtbl[i]);

}

/* testa operadores de adição */
int isaddop(char c)
{
    return (c == '+' || c == '-');

}

/* testa operadores de multiplicação */
int ismulop(char c)
{
    return (c == '*' || c == '/');

}

/* testa operadores OU */
int isorop(char c)
{
    return (c == '|' || c == '~');

}

/* testa operadores relacionais */
int isrelop(char c)
{
    return (strchr("=#<>", c) != NULL);

}

/* pula caracteres em branco */
void skipwhite()
{
    while (look == ' ' || look == '\t')
        nextchar();

}

/* reconhece uma quebra de linha */
void newline()
{
    if (look == '\n')
        nextchar();

}

/* verifica se look combina com caracter esperado */
void match(char c)
{
    char s[2];
    if (look == c)
        nextchar();

    else {
        s[0] = c; /* uma conversao rápida (e feia) */
        s[1] = '\0'; /* de um caracter em string */
        expected(s);

    }
    skipwhite();

}

/* analisa e traduz um nome (identificador ou palavra-chave) */
char getname()
{

    char name;
    if (!isalpha(look))
        expected("Name");

    name = toupper(look);
    nextchar();
    skipwhite();
    return name;

}

/* analisa e traduz um número inteiro */
long getnum()
{
    char val;

    if (!isdigit(look))
        expected("Integer");

    //num = look;
    //nextchar();
    val = 0;

    while (isdigit(look)) {
        val *= 10;
        val += look - '0';
        nextchar();

    }
    skipwhite();
    return val;

}

/* retorna o tipo de um identificador */
char symtype(char name)
{
    return symtbl[name - 'A'];

}

/* verifica se "name" consta na tabela de símbolos */
char intable(char name)
{
    return (symtbl[name - 'A'] != '?');

}

/* avisa a respeito de um identificador desconhecido */
void duplicated(char name)
{
    fprintf(stderr, "Error: Duplicated identifier %c\n", name);
    exit(1);

}

/* verifica se um identificador já foi declarado */
void checkdup(char name)
{
    if (intable(name))
        duplicated(name);

}

/* adiciona novo identificador à tabela de símbolos */
void addsymbol(char name, char type)
{
    checkdup(name);
    symtbl[name - 'A'] = type;

}

/* avisa a respeito de uma palavra-chave desconhecida */
void unrecognized(char name)
{
    fprintf(stderr, "Error: Unrecognized keyword %c\n", name);
    exit(1);

}

/* aloca espaço de armazenamento para variável */
void asm_allocvar(char name, char type)
{
    //addsymbol(name, 'v');
    int btype; /* tamanho em bytes */
    switch (type) {
        case 'b':

        case 'w':
            btype = type;
            break;

        case 'l':
            btype = 'd';
            break;

    }
    printf("%c: d%c 0\n", name, btype);

}

/* aloca espaço de armazenamento para variável */
void alloc(char name, char type)
{
        addsymbol(name, type);
        asm_allocvar(name, type);

}

/* analiza e traduz a declaração de uma variável */
void decl()
{
    //match('v');
    //asm_allocvar(getname());
    char type = look;
    nextchar();
    alloc(getname(), type);

}

/* analiza e traduz as declarações globais */
void topdecls()
{
    while (look != 'B') {
        switch (look) {
            case 'b':

            case 'w':

            case 'l':
                decl();
                break;

            default:
                unrecognized(look);
                break;

        }
        newline();

    }

}

/* gera código para carregar variável de acordo com o tipo */
void asm_loadvar(char name, char type)
{
    switch (type) {
        case 'b':
            //printf("\txor dx, dx\n");
            //printf("\txor ax, ax\n");
            printf("\tmov al, %c\n", name);
            //printf("\tmov al, %c\n", name);
            //printf("\tcbw\n");
            //printf("\tcwd\n");
            break;

        case 'w':
            //printf("\txor dx, dx\n");
            printf("\tmov ax, %c\n", name);
            //printf("\tmov ax, %c\n", name);
            //printf("\tcwd\n");
            break;

        case 'l':
            printf("\tmov dx, word ptr [%c+2]\n", name);
            printf("\tmov ax, word ptr [%c]\n", name);

    }

}

/* converte tipo origem para destino */
void asm_convert(char src, char dst)
{
    if (src == dst)
        return;

    if (src == 'b')
        printf("\tcbw\n");

    if (dst == 'l')
        printf("\tcwd\n");

}

/* reconhece um tipo de variável válido */
int isvartype(char c)
{
    return (c == 'b' || c == 'w' || c == 'l');

}

/* avisa a respeito de um identificador que não é variável */
void notvar(char name)
{
    fprintf(stderr, "Error: %c is not a variable\n", name);
    exit(1);

}

/* pega o tipo da variável da tabela de símbolos */
char vartype(char name)
{
    char type;
    type = symtype(name);
    if (!isvartype(type))
        notvar(name);

    return type;

}

/* carrega variável */
char loadvar(char name)
{
    char type = vartype(name);
    asm_loadvar(name, type);
    return type;

}

/* gera código para armazenar variável de acordo com o tipo */
void asm_storevar(char name, char type)
{
    switch (type) {
        case 'b':
            printf("\tmov %c, al\n", name);
            break;

        case 'w':
            printf("\tmov %c, ax\n", name);
            break;

        case 'l':
            printf("\tmov word ptr [%c+2], dx\n", name);
            printf("\tmov word ptr [%c], ax\n", name);

    }

}

/* armazena variável */
void storevar(char name, char srctype)
{
    char vartype1 = vartype(name);
    asm_convert(srctype, vartype1);
    asm_storevar(name, vartype1);

}

/* analisa e traduz uma expressão */
char expression()
{
    char type;
    if (isalpha(look))
        //type = loadvar(getname());
        type = unop();

    else
        type = loadnum(getnum());
        type = term();

    while (isaddop(look)) {
        asm_push(type);
        switch (look) {
            case '+':
                type = add(type);
                break;

            case '-':
                type = subtract(type);
                break;

        }

    }
    return type;

}

char unop()
{
    asm_clear();
    return 'w';

}

/* analisa e traduz uma atribuição */
void assignment()
{
    char name, type;
    name = getname();
    match('=');
    type = expression();
    storevar(name, type);

}

/* analisa traduz um bloco de comandos */
void block()
{
    while (look != '.') {
        assignment();
        newline();

    }

}

/* carrega uma constante no registrador primária */
char loadnum(long val)
{
    char type;
    if (val >= -128 && val <= 127)
        type = 'b';

    else if (val >= -32768 && val <= 32767)
        type = 'w';

    else
        type = 'l';

    asm_loadconst(val, type);
    return type;

}

void asm_loadconst(long val, char type)
{
    switch (type) {
        case 'b':
            printf("\tmov al, %d\n", (int) val);
            break;

        case 'w':
            printf("\tmov ax, %d\n", (int) val);
            break;

        case 'l':
            printf("\tmov dx, %u\n", val >> 16);
            printf("\tmov ax, %u\n", val & 0xFFFF);
                break;

    }

}

/* coloca valor na pilha */
void asm_push(char type)
{
    if (type == 'b')
        printf("\tcbw\n"); /* só é possível empilhar "word"s */

    if (type == 'l')
        printf("\tpush dx\n");

    printf("\tpush ax\n");

}

/* reconhece e traduz uma soma */
char add(char type)
{
    match('+');
    return asm_popadd(type, term());

}

/* reconhece e traduz uma subtração */
char subtract(char type)
{
    match('-');
    return asm_popsub(type, term());

}

/* coloca em registrador(es) secundário(s) valor da pilha */
void asm_pop(char type)
{
    printf("\tpop bx\n");

    if (type == 'l')
        printf("\tpop cx\n");

}

/* gera código para trocar registradores primário e secundário */
void asm_swap(char type)
{
    switch (type) {
        case 'b':
            printf("\txchg al, bl\n");
            break;

        case 'w':
            printf("\txchg ax, bx\n");
            break;

        case 'l':
            printf("\txchg ax, bx\n");
            printf("\txchg dx, cx\n");
            break;

    }

}

/* faz a promoção dos tipos dos operandos e inverte a ordem dos mesmos */
char asm_sametype(char t1, char t2, int ord_matters)
{
    int swaped = 0;
    int type = t1;

    if (t1 != t2) {
        if ((t1 == 'b') || (t1 == 'w' && t2 == 'l')) {
            type = t2;
            asm_swap(type);
            asm_convert(t1, t2);
            swaped = 1;

        } else {
            type = t1;
            asm_convert(t2, t1);

        }

    }

    if (!swaped && ord_matters)
        asm_swap(type);

    return type;

}

/* soma valor na pilha com valor no reg. primário */
char asm_popadd(char t1, char t2)
{
    char type;
    asm_pop(t1);
    type = asm_sametype(t1, t2, 0);

    switch (type) {
        case 'b':
            printf("\tadd al, bl\n");
            break;

        case 'w':
            printf("\tadd ax, bx\n");
            break;

        case 'l':
            printf("\tadd ax, bx\n");
            printf("\tadc dx, cx\n");
            break;

    }
    return type;

}

/* subtrai do valor da pilha o valor no reg. primário */
char asm_popsub(char t1, char t2)
{
    char type;
    asm_pop(t1);
    type = asm_sametype(t1, t2, 0);

    switch (type) {
        case 'b':
            printf("\tsub al, bl\n");
            break;

        case 'w':
            printf("\tsub ax, bx\n");
            break;

        case 'l':
            printf("\tsub ax, bx\n");
            printf("\tsbb dx, cx\n");

            break;

    }
    return type;

}

/* analisa e traduz um fator matemático */
char factor()
{
    char type;

    if (look == '(') {
        match('(');
        type = expression();
        match(')');

    } else if (isalpha(look))
        type = loadvar(getname());

    else
        type = loadnum(getnum());

    return type;

}

/* reconhece e traduz uma multiplicação */
char multiply(char type)
{
    match('*');
    return asm_popmul(type, factor());

}

/* reconhece e traduz uma multiplicação */
char divide(char type)
{
    match('/');
    return asm_popdiv(type, factor());

}

/* analisa e traduz um termo matemático */
char term()
{

    char type;
    type = factor();

    while (ismulop(look)) {
        asm_push(type);
        switch (look) {
            case '*':
                type = multiply(type);
                break;

            case '/':
                type = divide(type);
                break;

        }

    }

    return type;

}

/* multiplica valor na pilha com valor no reg. primário */
char asm_popmul(char t1, char t2)
{
    char type, multype;
    asm_pop(t1);
    type = asm_sametype(t1, t2, 0);
    multype = 'l';

    switch (type) {
        case 'b':
            printf("\tmul bl\n");
            multype = 'w';
            break;

        case 'w':
            printf("\tmul bx\n");
            break;

        case 'l':
            printf("\tcall MUL32\n");
            break;

    }
    return multype;

}

/* divide valor na pilha por valor do reg. primário */
char asm_popdiv(char t1, char t2)
{
    asm_pop(t1);
    /* se dividendo for 32-bits divisor deve ser também */

    if (t1 == 'l')
        asm_convert(t2, 'l');

    /* coloca operandos na ordem certa conforme o tipo */
    if (t1 == 'l' || t2 == 'l')
        asm_swap('l');

    else if (t1 == 'w' || t2 == 'w')
        asm_swap('w');

    else
        asm_swap('b');

    /* dividendo _REAL_ sempre será LONG...

        mas WORD se divisor for BYTE */
    if (t2 == 'b')
        asm_convert(t1, 'w');

    else
        asm_convert(t1, 'l');

    /* se um dos operandos for LONG, divisão de 32-bits */
    if (t1 == 'l' || t2 == 'l')
        printf("\tcall DIV32\n");

    else if (t2 == 'w') /* 32 / 16 */
        printf("\tidiv bx\n");

    else if (t2 == 'b') /* 16 / 8 */
        printf("\tidiv bl\n");

    /* tipo do quociente é sempre igual ao do dividendo */
    return t1;

}

/* zera o registrador primário */
void asm_clear()
{
    printf("\txor ax, ax\n");

}