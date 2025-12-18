#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static void * threadFunc(void *arg)
{
    printf("New thread started\n");

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    for (int i = 0; ; i++)
    {
        printf("%d\n", i);
        sleep(1); 
    }

    return NULL;
}

int main(void)
{
    printf("Main thread started\n");
    pthread_t t1;
    void *res;
    int s;

    s = pthread_create(&t1, NULL, threadFunc, NULL);
    if (s != 0) {
        fprintf(stderr, "pthread_create error\n");
        exit(EXIT_FAILURE);
    }

    sleep(3);

    s = pthread_cancel(t1);
    if (s != 0) {
        fprintf(stderr, "pthread_cancel error\n");
        exit(EXIT_FAILURE);
    }

    printf("Cancel request sent, but thread ignores it.\n");

    sleep(5);

    exit(EXIT_SUCCESS);
}
