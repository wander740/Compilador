/* main.c */
#include <stdio.h>
#include "input.h"
#include "output.h"
#include "errors.h"
#include "scanner1.h"
#include "parser.h"
#include "codegen.h"

int main()
{
    initInput();
    factor();

    return 0;

}