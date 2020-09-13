#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 4096

int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: <pathname1> <pathname2>\n");
        exit(1);
    }

    FILE *fp1 = NULL, *fp2 = NULL;

    if ((fp1 = fopen(argv[1], "r")) == NULL) {
        perror("fopen() 1");
        exit(1);
    }

    if ((fp2 = fopen(argv[2], "w")) == NULL) {
        fclose(fp1);
        perror("fopen() 2");
        exit(1);
    }

    char buf[BUFFSIZE];

    int n = 0;

    while ((n = fread(buf, 1, BUFFSIZE, fp1)) > 0)
        fwrite(buf, 1, n, fp2);

    fclose(fp1);
    fclose(fp2);

    exit(0);
}
