#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 10
int data[SIZE] = {1,2,3,4,5,6,7,8,9,10};

typedef struct {
    int start;
    int end;
    int sum;
} SumArgs;

void *sumFunc(void *arg) {
    SumArgs *args = (SumArgs *)arg;
    args->sum = 0;
    for (int i = args->start; i < args->end; i++) {
        args->sum += data[i];
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    SumArgs args1 = {0, SIZE/2, 0};  
    SumArgs args2 = {SIZE/2, SIZE, 0}; 

    if (pthread_create(&t1, NULL, sumFunc, &args1) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&t2, NULL, sumFunc, &args2) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    int total = args1.sum + args2.sum;
    printf("First half sum = %d\n", args1.sum);
    printf("Second half sum = %d\n", args2.sum);
    printf("Total sum = %d\n", total);

    return 0;
}
