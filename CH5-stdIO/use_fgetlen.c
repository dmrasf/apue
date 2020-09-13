/*
 * 统计文件字符个数
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    FILE* fp = NULL;

    if (argc != 2) {
        fprintf(stderr, "usage: <pathname>\n");
        exit(1);
    }

    if ((fp = fopen(argv[1], "r")) == 0) {
        perror("fopen()");
        exit(1);
    }

    /*int count = 0;*/
    /*while (fgetc(fp) != EOF)*/
    /*count++;*/
    /*printf("count = %d\n", count);*/

    if (fseek(fp, 0, SEEK_END) == -1) {
        perror("fseek()");
        exit(1);
    }

    printf("len = %ld\n", ftell(fp));

    exit(0);
}
