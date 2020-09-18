#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define PROC_NUM 10
#define PATH_NAME "/tmp/out"

static void add_func()
{
    FILE* fp;
    char buf[BUFSIZE];
    int fd;
    if ((fp = fopen(PATH_NAME, "r+")) == NULL) {
        perror("fopen");
    }

    fd = fileno(fp);
    if (fd < 0) {
        perror("fileno");
        exit(1);
    }
    lockf(fd, F_LOCK, 0);
    fgets(buf, BUFSIZE, fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", atoi(buf) + 1);
    fflush(fp);
    lockf(fd, F_ULOCK, 0);
    fclose(fp);
}

int main(void)
{
    int i, err, j;
    pid_t pid;

    for (i = 0; i < PROC_NUM; i++) {
        pid = fork();
        if (pid < 0) {
            for (j = 0; j < i; j++)
                wait(NULL);
            exit(1);
        }
        if (pid == 0) {
            add_func();
            exit(0);
        }
    }

    for (i = 0; i < PROC_NUM; i++)
        wait(NULL);

    exit(0);
}
