#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void int_handle(int s)
{
    write(1, "!", 1);
}

static __sighandler_t before;

void re(void)
{
    signal(SIGALRM, before);
}

int main(void)
{
    signal(SIGINT, int_handle);

    alarm(3);

    before = signal(SIGALRM, int_handle);
    atexit(re);

    for (int i = 0; i < 10; i++) {
        write(1, "*", 1);
        sleep(1);
    }
    write(1, "\n", 1);

    exit(0);
}
