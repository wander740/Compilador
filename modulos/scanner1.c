/* scanner1.c */
#include <ctype.h>
#include "input.h"
#include "errors.h"
#include "scanner1.h"

/* reconhece um operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-' || c == '|' || c == '~');

}

/* reconhece um operador multiplicativo */
int isMulOp(char c)
{
    return (c == '*' || c == '/' || c == '&');

}

/* verifica se caracter combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);

    nextChar();                       

}

/* retorna um identificador */
void getName(char *name)
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
void getNum(char *num)
{
    //long num = 0;
    int i;

    if (!isdigit(look))
        expected("Integer");

    /*do {
        num *= 10;
        num += look - '0';
        nextChar();                       
    } while (isdigit(look));*/

    //return num;
    for (i = 0; isdigit(look); i++) {
		if (i >= MAXNUM)
			error("Integer too large.");
            
		num[i] = look;
		nextChar();
	}
	num[i] = '\0';
}

/* reconhece um operador aditivo */
int isAddOp(char c)
{
	return (c == '+' || c == '-' || c == '|' || c == '~');

}

/* reconhece um operador multiplicativo */
int isMulOp(char c)
{
    return (c == '*' || c == '/' || c == '&');

}