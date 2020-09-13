#include "../apue.h"
#include <dirent.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    DIR           * dp;
    struct dirent * dirp;

    for (int i = 0; i < argc; i++) {
        printf("%s", argv[i]);
        if (i == argc - 1)  printf("\n");  else  printf(" ");
    }

    if (argc != 2) {
        err_quit("usage: ls directory_name");
    }

    if ((dp = opendir(argv[1])) == NULL) {
        err_sys("Can't open %s", argv[1]);
    }

    while ((dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name);
        printf("%ld\n", dirp->d_off);
        /*printf("%c\n", dirp->d_type);*/
    }

    closedir(dp);
    exit(0);
}
