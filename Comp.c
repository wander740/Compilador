#include <stdio.h>
#include <stdlib.h>
//stdarg tem estruturas para lidar com varios parametros
//de função
#include <stdarg.h>
#include <ctype.h>

#define MAXNAME 30
#define MAXNUM 5

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void init();
void nextChar();
// ... é usado para indicar que podemos ter uma quantidade
// arbitraria de parametros
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName1();
char getNum1();
void emit(char *fmt, ...);
void expression();
void term();
void add();
void subtract();
void factor();
void multiply();
void divide();
int isAddOp(char c);
void ident();
void assignment();
void skipWhite();
void getName(char *name);
void getNum(char *num);

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    assignment();
    if (look != '\n')
        expected("NewLine");

    return 0;
}


// recebe o nome de um identificador
void getName(char *name)
{

    int i;
    if (!isalpha(look))
        expected("Name");

    for (i = 0; isalnum(look); i++) {
        if (i >= MAXNAME)
            fatal("Identifier too long!");
            name[i] = toupper(look);
            nextChar();

    }

    name[i] = '\0';
    skipWhite();

}

// recebe um número inteiro
void getNum(char *num)
{

    int i;

    if (!isdigit(look))
        expected("Integer");

    for (i = 0; isdigit(look); i++) {
        if (i >= MAXNUM)
            fatal("Integer too long!");

        num[i] = look;
        nextChar();

    }

    num[i] = '\0';
    skipWhite();

}

// recebe o nome de um identificador 
char getName1()
{
    char name;

    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();
    skipWhite();

    return name;
}

// recebe um número inteiro 
char getNum1()
{
    char num;

    //se não for um número gera uma erro
    if (!isdigit(look))
        expected("Integer");
    num = look;
    nextChar();
    skipWhite();

    return num;
}

/* inicialização do compilador */
void init()
{
    nextChar();
    //pula espaços em branco
    skipWhite();

}

/* lê próximo caracter da entrada */
void nextChar()
{
    //getchar obtem o próximo caracter do buffer de entrada
    look = getchar();
}

/* exibe uma mensagem de erro formatada */
void error(char *fmt, ...)
{
    //va_list guardo os argumentos que serão passados em
    //...
    va_list args;

    fputs("Error: ", stderr);

    //va_start manipula va_list, recebe list e depois o
    //último argumento empilhado da função no caso fmt
    va_start(args, fmt);
    //va_arg(args,int)
    vfprintf(stderr, fmt, args);

    //depois de ler todos os argumento de args
    va_end(args);

    fputc('\n', stderr);
}

/* exibe uma mensagem de erro formatada e sai */
void fatal(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);

    exit(1);
}

/* alerta sobre alguma entrada esperada */
void expected(char *fmt, ...)
{
    va_list args;

    fputs("Error: ", stderr);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputs(" expected!\n", stderr);

    exit(1);
}

/* verifica se entrada combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar();
    skipWhite();

}

/* analisa e traduz um comando de atribuição */
void assignment()
{
    //char name;
    //name = getName();

    char name[MAXNAME+1];
    getName(name);

    match('=');
    expression();
    emit("MOV [%s], AX", name);

}

/* emite uma instrução seguida por uma nova linha */
void emit(char *fmt, ...)
{
    va_list args;

    putchar('\t');

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    putchar('\n');
}

/* analisa e traduz uma expressão */
void term()
{
    factor();
    //primeiro verificamos mul. e ad. pois tem prioridade
    //em expressões numéricas
    while (look == '*' || look == '/') {
        emit("PUSH AX");
        switch(look) {
            case '*':
                multiply();
                break;
            case '/':
                divide();
                break;
            default:
                expected("MulOp");
                break;
        }
    }
}

/* reconhece e traduz uma expressão */
void expression()
{
    if (isAddOp(look))
        //XOR AX, AX -> será definido AX como zero
        //para mudar operações do tipo -1 para
        //0-1
        emit("XOR AX, AX");
    else
        term();

    while (isAddOp(look)) {
        //push coloca na pilha
        emit("PUSH AX");
        //emit("MOV BX, AX");
        switch(look) {
            case '+':
                add();
                break;
            case '-':
                subtract();
                break;
            default:
                expected("AddOp");
                break;
        }
    }
}

/* reconhece operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* reconhece e traduz uma adição */
void add()
{
    match('+');
    term();
    //retira da pinha e coloca em BX
    emit("POP BX");
    emit("ADD AX, BX");
}

/* reconhece e traduz uma subtração */
void subtract()
{
    match('-');
    term();
    emit("POP BX");
    emit("SUB AX, BX");
    //muda sinal de AX
    emit("NEG AX");
}

/* analisa e traduz um identificador */
void ident()
{

    //char name;
    //name = getName();
    
    //para nomes com mais de uma letra
    char name[MAXNAME+1];
    getName(name);

    //verifica função
    if (look == '(') {

        match('(');
        match(')');
        emit("CALL %s", name);

    } else
        //isalpha verifica se look o caractere
        //é um algabeto (a-z e A-Z)
        //emit("MOV AX, [%c]",getName());
        //move o valor contido em name para AX
        emit("MOV AX, [%s]", name);

}

/* analisa e traduz um fator matemático */
void factor()
{
    //mais de uma casa decimal
    char num[MAXNUM+1];
    
    if (look == '(') {
        match('(');
        expression();
        match(')');
    //verifica se é uma variavel
    }else if(isalpha(look))
        ident();
    else
        getNum(num);
        emit("MOV AX, %s", num);

        //MOV coloca c no registrador AX
        //emit("MOV AX, %c", getNum());
}

//multiplicação e a divisão usam o par de 
//registradores DX:AX para armazenar os valores

/* reconhece e traduz uma multiplicação */
void multiply()
{
    match('*');
    factor();
    emit("POP BX");
    emit("IMUL BX");
}

/* reconhece e traduz uma divisão */
void divide()
{
    match('/');
    factor();
    emit("POP BX");
    emit("XCHG AX, BX");
    //CWD expande o valor de AX para DX:AX
    emit("CWD");
    emit("IDIV BX");
}

/* pula caracteres de espaço */

void skipWhite()
{
    while (look == ' ' || look == '\t')
        nextChar();

}