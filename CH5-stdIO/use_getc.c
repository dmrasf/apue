#include "../apue.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int c;

    while ((c = fgetc(stdin)) != EOF) {
        if (putc(c, stdout) == EOF)
            err_sys("output error");
    }

    if (ferror(stdin))
        err_sys("input error");

    exit(0);
}
