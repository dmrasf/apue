#include <stdio.h>
#include <stdlib.h>

static void my_exit1(void);
static void my_exit2(void);

/*extern char** environ;*/

int main(void)
{
    if (atexit(my_exit1) != 0) {
        perror("atexit()");
        exit(1);
    }
    if (atexit(my_exit2) != 0) {
        perror("atexit()");
        exit(1);
    }
    if (atexit(my_exit2) != 0) {
        perror("atexit()");
        exit(1);
    }

    printf("done\n");

    /*printf("%s\n", environ[0]);*/
    /*while (*environ) {*/
    /*fprintf(stdout, "%s\n", *environ);*/
    /*environ++;*/
    /*}*/

    exit(0);
}

static void my_exit1(void)
{
    printf("first exit handler\n");
}

static void my_exit2(void)
{
    printf("second exit handler\n");
}
