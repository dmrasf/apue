#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

static int mydaemon(void);

int main(void)
{
    // 使用 journalctl -f 查看
    openlog("mydeamon", LOG_PID, LOG_DAEMON);

    // 开启一个守护进程
    if (mydaemon() < 0) {
        syslog(LOG_ERR, "daemon error");
        exit(1);
    } else {
        syslog(LOG_INFO, "daemon successded");
    }

    FILE* fp;

    if ((fp = fopen("/tmp/out", "w")) == NULL) {
        syslog(LOG_ERR, "fopen error: %s", strerror(errno));
        exit(1);
    }

    syslog(LOG_INFO, "%s was opened", "/tmp/null");

    for (int i = 1;; i++) {
        fprintf(fp, "%d\n", i);
        fflush(fp);
        sleep(1);
    }

    fclose(fp);
    closelog();

    exit(0);
}

static int mydaemon(void)
{
    pid_t pid;
    int fd;

    pid = fork();

    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        // 重定向到/dev/null
        if ((fd = open("/dev/null", O_RDWR)) == -1) {
            return -1;
        }
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        if (fd > 2)
            close(fd);

        setsid();

        // 挂载的设备
        chdir("/");

        return 0;
    } else {
        exit(0);
    }
}
