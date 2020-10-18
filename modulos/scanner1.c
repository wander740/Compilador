/* scanner1.c */
#include <ctype.h>
#include "input.h"
#include "scanner1.h"

/* reconhece um operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');

}

/* reconhece um operador multiplicativo */
int isMulOp(char c)
{
    return (c == '*' || c == '/');

}

/* verifica se caracter combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);

    nextChar();                       

}

/* retorna um identificador */
char getName()
{

    int i;

    if (!isalpha(look))
        expected("Name");

    for (i = 0; isalnum(look); i++) {
        if (i >= MAXNAME)
            error("Identifier too long.");

        name[i] = toupper(look);
        nextChar();

    }

    name[i] = '\0';

}

/* retorna um número */
char getNum()
{
    long num = 0;

    if (!isdigit(look))
        expected("Integer");

    do {
        num *= 10;
        num += look - '0';
        nextChar();                       

    } while (isdigit(look));

    return num;

}