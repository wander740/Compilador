/* scanner1.h */

#ifndef _SCANNER1_H
#define _SCANNER1_H

#define MAXNAME 60
#define MAXNUM 60

int isAddOp(char c);
int isMulOp(char c);
void match(char c);
void getName(char *name);
void getNum(char *num);

#endif