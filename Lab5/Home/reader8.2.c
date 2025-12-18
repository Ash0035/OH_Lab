// reader_mmap.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    if (argc != 2) { fprintf(stderr, "Usage: %s <file>\n", argv[0]); return 1; }
    const char *file = argv[1];
    int fd = open(file, O_RDONLY);
    if (fd == -1) { perror("open"); return 1; }
    struct stat st;
    if (fstat(fd, &st) == -1) { perror("fstat"); close(fd); return 1; }
    size_t len = st.st_size;
    char *map = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) { perror("mmap"); close(fd); return 1; }
    printf("Reader sees: %s\n", map);
    munmap(map, len);
    close(fd);
    return 0;
}

