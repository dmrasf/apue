#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*#define FCNTL*/
#define MAXFD 4096

int mydup2(int fd, int fd2);

int main(int argc, char* argv[])
{
    int fd, fd2;

    fd = open(argv[1], O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

    printf("fd: %d\n", fd);
    fd2 = mydup2(fd, 6);
    printf("fd2: %d\n", fd2);

    if (write(fd2, "hello world!\n", 13) == -1) {
        perror("write()");
        exit(1);
    }

    return 0;
}

int mydup2(int fd, int fd2)
{
#ifdef FCNTL
    // 使用 fcntl
    int newfd;
    if (fd == fd2)
        return fd;
    else {
        close(fd2);
        if ((newfd = fcntl(fd, F_DUPFD, fd2)) == -1)
            err_quit("dup2 error");
        else
            return newfd;
    }
#else
    // 使用 dup
    if (fd == fd2)
        return fd;

    // 存放fd
    struct Node {
        int fd;
        struct Node* next;
    };

    int newfd;
    struct Node* head = (struct Node*)malloc(sizeof(struct Node));
    struct Node* cur = head;

    if (fd2 > MAXFD || fd2 < 3) {
        fprintf(stderr, "fd\n");
        exit(1);
    }

    while ((newfd = dup(fd)) < fd2) {
        printf("create newfd: %d\n", newfd);
        struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
        temp->fd = newfd;
        temp->next = NULL;
        cur->next = temp;
        cur = cur->next;
    }

    while (head->next != NULL) {
        printf("close newfd: %d\n", head->next->fd);
        close(head->next->fd);
        cur = head->next;
        free(head);
        head = cur;
    }

    return newfd;
#endif
}
