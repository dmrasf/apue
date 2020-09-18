#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage...\n");
        exit(1);
    }

    int fd;
    struct stat st;
    long count;
    char *buf;

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("open");
        exit(1);
    }

    if(fstat(fd, &st) < 0) {
        perror("fstat");
        exit(1);
    }

    buf = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    close(fd);

    for (int i = 0; i < st.st_size; i++)
        if (buf[i] == 'a')
            count++;

    printf("count = %ld\n", count);

    exit(0);
}
