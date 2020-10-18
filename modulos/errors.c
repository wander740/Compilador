/* errors.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errors.h"

void error(char *fmt, ...)
{
    va_list args;
    fputs("Error: ", stderr);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    exit(1);

}

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