#include "../apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (3 != argc)
        err_quit("./a.out <filename> <newfilename>");

    if (link(argv[1], argv[2]) < 0)
        err_quit("link error");

    exit(0);
}
