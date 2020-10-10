#include <stdio.h>
#include <stdlib.h>
//stdarg tem estruturas para lidar com varios parametros
//de função
#include <stdarg.h>
#include <ctype.h>

#define MAXNAME 30
#define MAXNUM 5

char look; /* O caracter lido "antecipadamente" (lookahead) */
int labelCount; /* Contador usado pelo gerador de rótulos */

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
void other();
void program();
void block();
int newLabel();
int postLabel(int lbl);
void doIf();
void condition();
void doLoop();
void doFor();
void expression1();
void doDo();
void doBreak(int );
int isBoolean(char c);
int getBoolean();
void boolTerm();
void boolOr();
void boolXor();
void boolExpression();
int isOrOp(char c);
void notFactor();
void relation();
int isRelOp(char c);
int isMulOp(char c);
void newLine();

/* PROGRAMA PRINCIPAL */
int main()
{
    labelCount = 0;
    init();
    //assignment();
    //if (look != '\n')
    //    expected("NewLine");
    program();
    //boolExpression();

    return 0;
}

/* gera um novo rótulo único */
int newLabel()
{
    return labelCount++;
}

/* emite um rótulo */
int postLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* analisa e traduz um comando BREAK */
void doBreak(int exitLabel)
{
        match('b');
        if (exitLabel == -1)
            fatal("No loop to break from");
        emit("JMP L%d", exitLabel);
}

/* analisa e traduz um comando DO */
void doDo()
{
    int l1, l2;

    match('d');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("MOV CX, AX");
    postLabel(l1);
    emit("PUSH CX");
    block(l2);
    emit("POP CX");
    emit("LOOP L%d", l1);
    emit("PUSH CX");
    postLabel(l2);
    emit("POP CX");
}

//IF <condição> <bloco> ENDIF
/* analisa e traduz um comando IF */
void doIf(int exitLabel)
{
    int l1,l2;

    //i indica if
    match('i');
    condition();
    l1 = newLabel();
    l2 = l1;
    //condition();
    emit("JZ L%d", l1);
    block(exitLabel);

    //confere o comendo else
    if (look == 'l') {
        match('l');
        l2 = newLabel();
        emit("JMP L%d", l2);
        postLabel(l1);
        block(exitLabel);
    }

    //e indica endif
    match('e');
    postLabel(l2);
}

/* analisa e traduz um comando WHILE */
void doWhile()
{
    int l1, l2;

    match('w');
    l1 = newLabel();
    l2 = newLabel();
    postLabel(l1);
    condition();
    emit("JZ L%d", l2);
    block(l2);
    match('e');
    emit("JMP L%d", l1);
    postLabel(l2);
}

/* analisa e traduz um comando LOOP */
void doLoop()
{
    int l1, l2;

    match('p');
    l1 = newLabel();
    l2 = newLabel();
    postLabel(l1);
    block(l2);
    match('e');
    emit("JMP L%d", l1);
    postLabel(l2);
}

/* analisa e traduz um REPEAT-UNTIL*/
void doRepeat()
{
    int l1, l2;

    match('r');
    l1 = newLabel();
    l2 = newLabel();
    postLabel(l1);
    block(l2);
    match('u');
    condition();
    emit("JZ L%d", l1);
    postLabel(l2);
}

//FOR <ident> = <expr1> TO <expr2> <block> ENDFOR
/* analisa e traduz um comando FOR*/
void doFor()
{
    int l1, l2;
    char name;

    match('f');
    l1 = newLabel();
    l2 = newLabel();
    name = getName1();
    match('=');
    //pega o valor inicial
    expression1();
    match('t');
    //pré-decrementa
    emit("DEC AX");
    //salva o valor do contador
    emit("MOV [%c], AX", name);
    //pega o expressionlimite superior
    expression1();
    //salva na pilha
    emit("PUSH AX");
    //L1:
    postLabel(l1);
    //coloca em AX
    emit("MOV AX, [%c]", name);
    //incrementa o contador
    emit("INC AX");
    //salva o novo valor
    emit("MOV [%c], AX", name);
    //pega limite superior...
    emit("POP BX");
    //...mas devolve na pilha
    emit("PUSH BX");
    //compara contador com limite superior
    emit("CMP AX, BX");
    //termina se contador > limite superior
    emit("JG L%d", l2);
    block(l2);
    match('e');
    //próximo passo
    emit("JMP L%d", l1);
    //L2:
    postLabel(l2);
    //retira limite superior da pilha
    emit("POP AX");
}

void expression1()
{
    emit("# EXPR");
}

/* analisa e traduz uma condição */
void condition()
{
    boolExpression();
    //emit("# condition");
}

/* reconhece e traduz um comando qualquer */
void other()
{
    emit("# %c", getName1());
}

/* analisa e traduz um programa completo */
void program()
{
    block(-1);
    //if (look != 'e')
    //    expected("End");
    match('e');
    emit("END");
}

/* analisa e traduz um bloco de comandos */
void block(int exitLabel)
{
    //enquanto for diferente de end
    //while (look != 'e' && look != 'l') {
    int follow;
    follow = 0;
    while(!follow){
        newLine();
        switch (look) {
            case 'i':
                doIf(exitLabel);
                break;
            case 'w':
                doWhile();
                break;
            case 'p':
                doLoop();
                break;
            case 'r':
                doRepeat();
                break;
            case 'f':
                doFor();
                break;
            case 'd':
                doDo();
                break;
            case 'b':
                doBreak(exitLabel); 
                break;
            case 'e':
            case 'l':
            case 'u':
                follow = 1;
                break;
            default:
                //other();
                assignment();
                break;
        }
        newLine();
    }
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
    char name;
    name = getName1();

    //char name[MAXNAME+1];
    //getName(name);

    match('=');
    expression();
    emit("MOV [%c], AX", name);

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
    while (isMulOp(look)) {
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

    char name;
    name = getName1();
    
    //para nomes com mais de uma letra
    //char name[MAXNAME+1];
    //getName(name);

    //verifica função
    if (look == '(') {
        match('(');
        match(')');
        //emit("CALL %s", name);
        emit("CALL %c", name);

    } else
        //isalpha verifica se look o caractere
        //é um algabeto (a-z e A-Z)
        //emit("MOV AX, [%c]",getName());
        //move o valor contido em name para AX
        //emit("MOV AX, [%s]", name);
        emit("MOV AX, [%c]", name);

}

/* analisa e traduz um fator matemático */
void factor()
{
    //mais de uma casa decimal
    //char num[MAXNUM+1];
    
    if (look == '(') {
        match('(');
        expression();
        match(')');
    //verifica se é uma variavel
    }else if(isalpha(look))
        ident();
    else
        //getNum(num);
        //emit("MOV AX, %s", num);

        //MOV coloca c no registrador AX
        emit("MOV AX, %c", getNum1());
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

/* reconhece uma literal Booleana */
int isBoolean(char c)
{
    return (c == 'T' || c == 'F');
}

/* recebe uma literal Booleana */
int getBoolean()
{
    int boolean;

    if (!isBoolean(look))
        expected("Boolean Literal");
    boolean = (look == 'T');
    nextChar();

    return boolean;
}

/* analisa e traduz uma expressão Booleana */
void boolFactor()
{
    if (isBoolean(look)) {
        if (getBoolean())
            emit("MOV AX, -1");
        else
            emit("MOV AX, 0");
    } else
        relation();
}

/* reconhece e traduz um operador OR */
void boolOr()
{
        match('|');
        boolTerm();
        emit("POP BX");
        emit("OR AX, BX");
}

/* reconhece e traduz um operador XOR */
void boolXor()
{
        match('~');
        boolTerm();
        emit("POP BX");
        emit("XOR AX, BX");
}

/* analisa e traduz uma expressão booleana */
void boolExpression()
{
        boolTerm();
        while (isOrOp(look)) {
                emit("PUSH AX");
                switch (look) {
                  case '|':
                        boolOr();
                        break;
                  case '~' :
                        boolXor();
                        break;
                }
        }
}

/* reconhece um operador OU */
int isOrOp(char c)
{
    return (c == '|' || c == '~');
}

int isMulOp(char c)
{
    return (c == '*' || c == '/');
}

/* analisa e traduz um termo booleano*/
void boolTerm()
{
    notFactor();
    while (look == '&') {
        emit("PUSH AX");
        match('&');
        notFactor();
        emit("POP BX");
        emit("AND AX, BX");
    }
}

/* analisa e traduz um fator booleno com NOT opcional */
void notFactor()
{
        if (look == '!') {
            match('!');
            boolFactor();
            emit("NOT AX");
        } else
            boolFactor();
}

/* reconhece operadores relacionais */
int isRelOp(char c)
{
    //diferente é o #
    return (c == '=' || c == '#' || c == '<' || c == '>');
}

/* reconhece e traduz um operador de igualdade */
void equals()
{
    int l1, l2;

    match('=');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JE L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de não-igualdade */
void notEquals()
{
    int l1, l2;

    match('#');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JNE L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de maior que */
void greater()
{
    int l1, l2;

    match('>');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JG L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de menor que */
void less()
{
    int l1, l2;

    match('<');
    l1 = newLabel();
    l2 = newLabel();
    expression();
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JL L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* analisa e traduz uma relação */
void relation()
{
    expression();
    if (isRelOp(look)) {
        emit("PUSH AX");
        switch (look) {
            case '=':
                equals();
                break;
            case '#':
                notEquals();
                break;
            case '>':
                greater();
                break;
            case '<':
                less();
                break;
        }
    }
}

/* reconhece uma linha em branco */
void newLine()
{
    if (look == '\n')
        nextChar();
}