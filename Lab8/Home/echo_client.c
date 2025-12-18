#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 6000
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sfd;
    struct sockaddr_in server;
    char buf[BUF_SIZE];

    if (argc < 3) {
        printf("Usage: %s <server-ip> <message>\n", argv[0]);
        exit(1);
    }

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    connect(sfd, (struct sockaddr*)&server, sizeof(server));

    write(sfd, argv[2], strlen(argv[2]));
    ssize_t n = read(sfd, buf, BUF_SIZE);
    buf[n] = '\0';

    printf("Server replied: %s\n", buf);
}

