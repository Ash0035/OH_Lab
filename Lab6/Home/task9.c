#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 10
int data[SIZE];

void *fillEven(void *arg) {
    for (int i = 1; i < SIZE; i += 2) {
        data[i] = i + 1;
    }
    return NULL;
}

int main(void) {
    pthread_t t1;
    int s;

    s = pthread_create(&t1, NULL, fillEven, NULL);
    if (s != 0) {
        fprintf(stderr, "pthread_create error\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SIZE; i += 2) {
        data[i] = i + 1; 
    }

    s = pthread_join(t1, NULL);
    if (s != 0) {
        fprintf(stderr, "pthread_join error\n");
        exit(EXIT_FAILURE);
    }

    printf("Array: ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    return 0;
}