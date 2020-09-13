#include <stdio.h>
#include <stdlib.h>

int count_ones(unsigned);

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage error....\n");
        exit(1);
    }

    unsigned a = atoi(argv[1]);

    printf("%s has %d one\n", argv[1], count_ones(a));

    exit(0);
}

int count_ones(unsigned a)
{
    a = (a & 0x55555555) + ((a >> 1) & 0x55555555);
    a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
    a = (a & 0x0f0f0f0f) + ((a >> 4) & 0x0f0f0f0f);
    a = (a & 0x00ff00ff) + ((a >> 8) & 0x00ff00ff);
    a = (a & 0x0000ffff) + ((a >> 16) & 0x0000ffff);
    return a;
}
