#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATHSIZE 1024

static long long mydu(const char* path);
static int checkloop(const char* path);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage error\n");
        exit(1);
    }

    printf("%lld\n", mydu(argv[1]) / 2);

    exit(0);
}

static long long mydu(const char* path)
{
    static struct stat st;
    static char nextpath[PATHSIZE];
    static int f1 = 0, f2 = 0;
    glob_t pg;
    long long sum = 0;

    if (lstat(path, &st) == -1) {
        perror("lstat");
        exit(1);
    }

    if (!S_ISDIR(st.st_mode)) {
        return st.st_blocks;
    } else {
        strncpy(nextpath, path, PATHSIZE - 1);
        strncat(nextpath, "/*", PATHSIZE - 1);
        f1 = glob(nextpath, 0, NULL, &pg);
        strncpy(nextpath, path, PATHSIZE - 1);
        strncat(nextpath, "/.*", PATHSIZE - 1);
        f2 = glob(nextpath, GLOB_APPEND, NULL, &pg);

        // 空目录
        if (f1 == GLOB_NOMATCH && pg.gl_pathc == 1) {
            return st.st_blocks;
        } else if (f1 == GLOB_ABORTED || f2 == GLOB_ABORTED) {
            fprintf(stderr, "nextpath /*");
            exit(1);
        }

        sum = st.st_blocks;
        for (int i = 0; i < pg.gl_pathc; i++) {
            if (checkloop(pg.gl_pathv[i]))
                sum += mydu(pg.gl_pathv[i]);
        }
        globfree(&pg);
        return sum;
    }
}

static int checkloop(const char* path)
{
    char* pos;
    if ((pos = strrchr(path, '/')) == NULL) {
        fprintf(stderr, "strrchr error");
        exit(1);
    }
    if (strcmp(pos + 1, ".") == 0 || strcmp(pos + 1, "..") == 0)
        return 0;
    return 1;
}
