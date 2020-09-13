#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void sig_alarm(int signo)
{
}

unsigned int mysleep(unsigned int seconds)
{
    if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
        return seconds;
    }
    alarm(seconds);
    pause();
    return alarm(0);
}

int main(void)
{

    mysleep(3);

    exit(0);
}
