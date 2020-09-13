#include "mytbf.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    int fd;
    do {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            if (errno != EINTR) {
                perror("open");
                exit(1);
            }
        }
    } while (fd < 0);

    struct mytbf_t* mt;
    if ((mt = mytbf_init(1, 100)) == NULL) {
        fprintf(stderr, "mytbf_init\n");
        exit(1);
    }

    char buf[BUFSIZE];
    int cnt, ret, pos, size;
    while (1) {
        if ((size = mytbf_fetchtoken(mt, BUFSIZE)) < 0) {
            fprintf(stderr, "%s\n", strerror(-size));
            exit(1);
        }

        while ((cnt = read(fd, buf, size)) < 0) {
            if (errno != EINTR) {
                perror("write");
                break;
            }
            continue;
        }
        if (cnt == 0)
            break;

        if (size > cnt)
            mytbf_returntoken(mt, size - cnt);

        pos = 0;
        while (cnt > 0) {
            while ((ret = write(1, buf + pos, cnt)) < 0) {
                if (errno != EINTR) {
                    perror("write");
                    break;
                }
                continue;
            }
            cnt -= ret;
        }
    }

    close(fd);

    exit(0);
}
