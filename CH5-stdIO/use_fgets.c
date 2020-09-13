#include "../apue.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char buf[4];

    FILE *fp;

    fp = fopen("./use_fgets.c", "r");

    while (fgets(buf, 4, fp) != NULL)
        if (fputs(buf, stdout) == EOF)
            err_sys("output error");

    if (ferror(stdin))
        err_sys("input error");

    exit(0);
}
