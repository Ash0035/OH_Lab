
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    if (argc != 3) { fprintf(stderr, "Usage: %s <file> <message>\n", argv[0]); return 1; }
    const char *file = argv[1];
    const char *msg = argv[2];

    int fd = open(file, O_RDWR | O_CREAT, 0666);
    if (fd == -1) { perror("open"); return 1; }
    size_t len = 4096;
    if (ftruncate(fd, len) == -1) { perror("ftruncate"); close(fd); return 1; }

    char *map = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    strncpy(map, msg, len-1);
    msync(map, len, MS_SYNC);
    munmap(map, len);
    close(fd);
    printf("Writer wrote: %s\n", msg);
    return 0;
}

