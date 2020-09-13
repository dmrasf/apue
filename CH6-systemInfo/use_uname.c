#include "../apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(void)
{
    struct utsname un;
    if (uname(&un) == -1)
        err_sys("uname error");

    printf("machine = %s\n", un.machine);
    printf("nodename = %s\n", un.nodename);
    printf("release = %s\n", un.release);
    printf("sysname = %s\n", un.sysname);
    printf("version = %s\n", un.version);

    exit(0);
}
