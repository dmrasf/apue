#include <dirent.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATHSIZE 1024

static long long mydu2(const char* path);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage error\n");
        exit(1);
    }

    printf("%lld\n", mydu2(argv[1]) / 2);

    exit(0);
}

static long long mydu2(const char* path)
{
    char nextpath[PATHSIZE];
    struct stat st;
    struct dirent* dirp;
    DIR* dp;
    long long sum = 0;

    if (lstat(path, &st) == -1) {
        perror("lstat");
        exit(1);
    }

    if (!S_ISDIR(st.st_mode)) {
        return st.st_blocks;
    } else {
        if ((dp = opendir(path)) == NULL) {
            perror("opendir error");
            exit(1);
        }

        sum = st.st_blocks;
        while ((dirp = readdir(dp)) != NULL) {
            if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
                continue;
            strncpy(nextpath, path, PATHSIZE - 1);
            strncat(nextpath, "/", PATHSIZE - 1);
            strncat(nextpath, dirp->d_name, PATHSIZE - 1);
            sum += mydu2(nextpath);
        }

        if (closedir(dp) < 0)
            fprintf(stderr, "closedir error\n");

        return sum;
    }
}
