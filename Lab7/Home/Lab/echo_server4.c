#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SV_SOCK_PATH "/tmp/echo_server"
#define BUF_SIZE 1024
#define BACKLOG 5

int main(void) {
    int sfd, cfd;
    struct sockaddr_un addr;
    char buf[BUF_SIZE];
    ssize_t numRead;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SV_SOCK_PATH);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path)-1);

    if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Echo server is running...\n");

    for (;;) {
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            continue;
        }

        printf("Client connected.\n");

        while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
            if (write(cfd, buf, numRead) != numRead) {
                perror("write");
                break;
            }
        }

        if (numRead == -1) {
            perror("read");
        }

        close(cfd);
        printf("Client disconnected.\n");
    }

    close(sfd);
    unlink(SV_SOCK_PATH);
    return 0;
}
