#include "../apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (chdir("/tmp") < 0)
        err_sys("chdir failes");
    printf("chdir to /tmp\n");
    exit(0);
}
