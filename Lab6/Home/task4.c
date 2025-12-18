#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int glob = 0;

void *threadFunc(void *arg) {
    int loops = *(int *)arg;
    int loc, j;

    for (j = 0; j < loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2;
    int loops = (argc > 1) ? atoi(argv[1]) : 10000000;
    int *loopsP = malloc(sizeof(int));
    *loopsP = loops;

    int s;

    s = pthread_create(&t1, NULL, threadFunc, loopsP);
    if (s != 0) { fprintf(stderr, "pthread_create error\n"); exit(EXIT_FAILURE); }

    s = pthread_join(t1, NULL);
    if (s != 0) { fprintf(stderr, "pthread_join error\n"); exit(EXIT_FAILURE); }

    s = pthread_create(&t2, NULL, threadFunc, loopsP);
    if (s != 0) { fprintf(stderr, "pthread_create error\n"); exit(EXIT_FAILURE); }

    s = pthread_join(t2, NULL);
    if (s != 0) { fprintf(stderr, "pthread_join error\n"); exit(EXIT_FAILURE); }

    printf("Final glob value = %d\n", glob);

    free(loopsP);
    return 0;
}
