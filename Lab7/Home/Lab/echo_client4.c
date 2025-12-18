#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SV_SOCK_PATH "/tmp/echo_server"
#define BUF_SIZE 1024

int main(void) {
    int sfd;
    struct sockaddr_un addr;
    char buf[BUF_SIZE];
    ssize_t numRead;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path)-1);

    if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to echo server. Type messages (Ctrl+D to quit):\n");
    fflush(stdout);

    while (fgets(buf, BUF_SIZE, stdin) != NULL) {
        size_t len = strlen(buf);

        if (write(sfd, buf, len) != len) {
            perror("write");
            break;
        }

        numRead = read(sfd, buf, BUF_SIZE-1);  
        if (numRead > 0) {
            buf[numRead] = '\0';              
            printf("%s", buf);                
            fflush(stdout);                   
        } else if (numRead == 0) {
            printf("Server closed connection\n");
            break;
        } else {
            perror("read");
            break;
        }
    }

    close(sfd);
    return 0;
}
