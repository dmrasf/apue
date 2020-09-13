#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void prompt(void);
static void parse(char*, glob_t*);

int main(void)
{
    pid_t pid;
    char* line = NULL;
    size_t len = 0;
    glob_t gb;

    while (1) {

        prompt();

        if (getline(&line, &len, stdin) == -1) {
            fprintf(stderr, "getline() failes\n");
            free(line);
            break;
        }

        parse(line, &gb);

        pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            execvp(gb.gl_pathv[0], gb.gl_pathv);
            perror("exec");
            exit(1);
        } else {
            wait(NULL);
        }
    }

    exit(0);
}

static void prompt(void)
{
    printf("➜  $ ");
    fflush(NULL);
}

static void parse(char* line, glob_t* gb)
{
    char* arg;
    int i = 0;

    while (1) {
        arg = strsep(&line, " \t\n");
        if (arg == NULL)
            break;
        else if (arg[0] == '\0')
            continue;

        // 获取参数添加到glob_t结构里
        glob(arg, GLOB_NOCHECK | GLOB_APPEND * i, NULL, gb);
        i = 1;
    }
}
