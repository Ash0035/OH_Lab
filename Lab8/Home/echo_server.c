#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 6000
#define BUF_SIZE 1024

int main() {
    int sfd, cfd;
    struct sockaddr_in addr;
    char buf[BUF_SIZE];

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sfd, 5);

    printf("TCP Echo server working...\n");

    while (1) {
        cfd = accept(sfd, NULL, NULL);
        while (1) {
            ssize_t n = read(cfd, buf, BUF_SIZE);
            if (n <= 0) break;
            write(cfd, buf, n);
        }
        close(cfd);
    }
}

