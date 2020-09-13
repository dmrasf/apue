#include "../apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void getpwd(char* ptr, size_t size)
{
    if (getcwd(ptr, size) == NULL)
        err_sys("getcwd failed\n");
    printf("cwd = %s\n", ptr);
}

int main(int argc, char* argv[])
{
    char* ptr;
    size_t size;
    ptr = path_alloc(&size);

    getpwd(ptr, size);

    if (chdir("/tmp") < 0)
        err_sys("chdir failed\n");

    getpwd(ptr, size);

    exit(0);
}
