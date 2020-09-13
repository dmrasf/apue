#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAT "/etc/NetworkManager/conf.d/*"

int errfuc(const char*, int);

int main(int argc, char* argv[])
{
    glob_t pg;

    if (glob(PAT, 0, &errfuc, &pg) != 0) {
        fprintf(stderr, "glob() error");
        exit(1);
    }

    for (int i = 0; i < pg.gl_pathc; i++)
        puts(pg.gl_pathv[i]);

    globfree(&pg);

    exit(0);
}

int errfuc(const char* epath, int errno)
{
    puts(epath);
    fprintf(stderr, "error %s\n", strerror(errno));
    return 0;
}
