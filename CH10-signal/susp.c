#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void int_handle(int s)
{
    write(1, "!", 1);
}

int main(void)
{
    signal(SIGINT, int_handle);

    sigset_t sgs, tmp;

    sigaddset(&sgs, SIGINT);

    sigprocmask(SIG_BLOCK, &sgs, &tmp);
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 5; j++) {
            write(1, "*", 1);
            sleep(1);
        }
        write(1, "\n", 1);
        sigsuspend(&tmp);
    }

    exit(0);
}
