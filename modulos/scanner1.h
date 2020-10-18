/* scanner1.h */

#ifndef _SCANNER1_H
#define _SCANNER1_H

#define MAXNAME 60
#define MAXNUM 60

int isAddOp(char c);
int isMulOp(char c);
void match(char c);
char getName();
char getNum();

#endif