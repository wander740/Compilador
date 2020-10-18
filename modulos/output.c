/* output.c */

#include <stdio.h>
#include <stdarg.h>

void emit(char *fmt, ...)
{
    va_list args;
    putchar('\t');
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');

}