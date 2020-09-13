#include "../apue.h"
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
        err_quit("usage: ./a.out <pathname>");
    if (access(argv[1], R_OK) < 0)
        err_ret("access error for %s\n", argv[1]);
    else
        printf("read access OK\n");

    if (access(argv[1], W_OK) < 0)
        err_ret("access error for %s\n", argv[1]);
    else
        printf("write access OK\n");

    printf("uid: %d\n", getuid());

    return 0;
}
