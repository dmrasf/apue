#include "../apue.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3) 
        err_sys("./a.out <pathname> <length>");

    if (truncate(argv[1], atoi(argv[2])) < 0) 
        err_sys("truncate error");

    exit(0);
}
