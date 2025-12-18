#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <num_of_semaphores>\n", argv[0]);
        exit(1);
    }

    int nsems = atoi(argv[1]);

    // Ստեղծում ենք key ֆայլի հիման վրա
    key_t key = ftok("sem.h", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // Ստեղծում ենք սեմաֆորների հավաքածու
    int semid = semget(key, nsems, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    printf("Semaphore set created successfully.\n");
    printf("Key = 0x%x, ID = %d, nsems = %d\n", key, semid, nsems);
    return 0;
}

