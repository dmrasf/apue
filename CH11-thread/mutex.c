#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024
#define TH_NUM 10

static pthread_mutex_t mut;

static void* thread_func(void* ptr)
{
    FILE* fp;
    char buf[BUFSIZE];
    if ((fp = fopen("/tmp/out", "r+")) == NULL) {
        perror("fopen");
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&mut);

    fgets(buf, BUFSIZE, fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", atoi(buf) + 1);
    fclose(fp);

    pthread_mutex_unlock(&mut);

    pthread_exit(NULL);
}

int main(void)
{
    int i, err;
    pthread_t tids[TH_NUM], tid;

    pthread_mutex_init(&mut, NULL);

    for (i = 0; i < TH_NUM; i++) {
        if ((err = pthread_create(&tid, NULL, thread_func, NULL))) {
            fprintf(stderr, "%s\n", strerror(err));
        }
        tids[i] = tid;
    }

    for (i = 0; i < TH_NUM; i++)
        pthread_join(tids[i], NULL);

    pthread_mutex_destroy(&mut);

    exit(0);
}
