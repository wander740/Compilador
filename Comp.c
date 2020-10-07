#include <stdio.h>
#include <stdlib.h>
//stdarg tem estruturas para lidar com varios parametros
//de função
#include <stdarg.h>
#include <ctype.h>

#define MAXNAME 30
#define MAXNUM 5
#define MAXVAR 26
int var[MAXVAR];

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
void emit(char *fmt, ...);
int expression();
int term();
void add();
void subtract();
int factor();
void multiply();
void divide();
int isAddOp(char c);
int isMulOp(char c);
void ident();
void assignment();
void skipWhite();
char getName();
int getNum();
void initVar();
void newLine();
void input();
void output();

/* PROGRAMA PRINCIPAL */
int main()
{
    init();
    do {
        switch (look) {
            case '?':
                input();
                break;
            case '!':
                output();
                break;
            default:
                assignment();
                break;
        }
        newLine();
    } while (look != '.');

    return 0;
}


// recebe o nome de um identificador
char getName()
{

    char name;

        if (!isalpha(look))
                expected("Name");
        name = toupper(look);
        nextChar();

        return name;

}

// recebe um número inteiro
int getNum()
{

    int i;

    i = 0;

    if (!isdigit(look))
        expected("Integer");

    while (isdigit(look)) {
        i *= 10;
        i += look - '0';
        nextChar();
    }

    return i;

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
    char name;
    name = getName();
    match('=');
    var[name - 'A'] = expression();

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
int term()
{
    int val;

    val = getNum();
    while (isMulOp(look)) {
            switch (look) {
                case '*':
                    match('*');
                    val *= getNum();
                    break;
                case '/':
                    match('/');
                    val /= getNum();
                    break;
            }
    }

    return val;
}

/* reconhece e traduz uma expressão */
int expression()
{
    int val;

    if (isAddOp(look))
        val = 0;
    else
        val = getNum();

    while (isAddOp(look)) {
        switch (look) {
            case '+':
                match('+');
                val += getNum();
                break;
            case '-':
                match('-');
                val -= getNum();
                break;
        }
    }

    return val;
}

/* reconhece operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}

int isMulOp(char c){
    return (c == '*' || c == '/');
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
int factor()
{
    int val;

    if (look == '(') {
        match('(');
        val = expression();
        match(')');
    } else if (isalpha(look))
        val = var[getName() - 'A'];
    else
        val = getNum();

    return val;
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

void initVar()
{
    int i;

    for (i = 0; i < MAXVAR; i++)
        var[i] = 0;
}

/* captura um caracter de nova linha */
void newLine()
{
    if (look == '\n')
        nextChar();
}

/* interpreta um comando de entrada */
void input()
{
    char name;
    char buffer[20];

    match('?');
    name = getName();
    printf("%c? ", name);
    fgets(buffer, 20, stdin);
    var[name - 'A'] = atoi(buffer);
}

/* interpreta um comando de saída */
void output()
{
    char name;

    match('!');
    name = getName();
    printf("%c -> %d", name, var[name - 'A']);
}