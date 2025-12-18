#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h> 
#include <errno.h>
#include "sem.h"


int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <semid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int semid = atoi(argv[1]);
    struct sembuf sops;

    // Perform a semaphore operation with SEM_UNDO
    sops.sem_num = 0;       // use semaphore 0 in the set
    sops.sem_op = -1;       // decrement (P operation)
    sops.sem_flg = SEM_UNDO; // this is what we are testing

    printf("Process %d trying to decrement semaphore with SEM_UNDO...\n", getpid());

    if (semop(semid, &sops, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    printf("Semaphore decremented successfully (SEM_UNDO active).\n");
    printf("Now sleeping for 30 seconds... (try checking ipcs -s)\n");
    printf("If you kill this process, the kernel will automatically undo this operation.\n");

    sleep(30);

    printf("Woke up, now exiting normally.\n");
    exit(EXIT_SUCCESS);
}

