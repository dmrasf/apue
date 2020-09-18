#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MEMSIZE 1024

int main(void)
{
    char* ptr;
    pid_t pid;

    ptr = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        munmap(ptr, MEMSIZE);
        exit(1);
    }

    if (pid == 0) {
        printf("child: %p\n", ptr);
        strcpy(ptr, "Hello!");
        munmap(ptr, MEMSIZE);
        printf("child: %p\n", ptr);
        exit(0);
    } else {
        wait(NULL);
        printf("parent: %p\n", ptr);
        puts(ptr);
        munmap(ptr, MEMSIZE);
        printf("parent: %p\n", ptr);
        exit(0);
    }

    exit(0);
}
