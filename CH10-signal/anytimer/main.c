#include "anytime.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handler(void* arg)
{
    printf("%s", arg);
    fflush(NULL);
}

int main(int argc, char* argv[])
{
    anytime_init();

    int job1, job2, job3;

    if ((job1 = anytime_add(5, handler, "555")) < 0) {
        fprintf(stderr,"%s\n", strerror(-job1));
        exit(1);

    }
    anytime_add(2, handler, "222");
    anytime_add(7, handler, "777");

    while (1) {
        write(1, ".", 1);
        sleep(1);
    }

    exit(0);
}
