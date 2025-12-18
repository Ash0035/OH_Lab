/* tcp_chat_client.c
   Simple TCP chat client using select().
   Reads stdin to send to server and prints server messages to stdout.
   Usage: ./tcp_chat_client <server-ip> <server-port (optional)>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUF_SIZE 1024
#define DEFAULT_PORT 7000

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server-ip> [port]\n", argv[0]);
        return 1;
    }
    const char *server_ip = argv[1];
    int port = (argc >= 3) ? atoi(argv[2]) : DEFAULT_PORT;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { perror("socket"); return 1; }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd); return 1;
    }

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("connect");
        close(sockfd); return 1;
    }

    printf("Connected to %s:%d. Type messages and press Enter to send.\n", server_ip, port);
    printf("To quit: Ctrl+D (EOF) or Ctrl+C\n");

    fd_set rset;
    int maxfd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;
    char sendbuf[BUF_SIZE], recvbuf[BUF_SIZE];

    while (1) {
        FD_ZERO(&rset);
        FD_SET(STDIN_FILENO, &rset);
        FD_SET(sockfd, &rset);

        if (select(maxfd + 1, &rset, NULL, NULL, NULL) == -1) {
            if (errno == EINTR) continue;
            perror("select"); break;
        }

        // stdin ready -> send to server
        if (FD_ISSET(STDIN_FILENO, &rset)) {
            ssize_t n = read(STDIN_FILENO, sendbuf, sizeof(sendbuf));
            if (n <= 0) {
                // EOF or error -> exit
                printf("Closing connection.\n");
                break;
            }
            // send to server
            ssize_t w = write(sockfd, sendbuf, n);
            if (w != n) {
                perror("write to server");
                break;
            }
        }

        // socket ready -> receive from server
        if (FD_ISSET(sockfd, &rset)) {
            ssize_t n = read(sockfd, recvbuf, sizeof(recvbuf));
            if (n <= 0) {
                if (n == 0) {
                    printf("Server closed connection.\n");
                } else {
                    perror("read from server");
                }
                break;
            }
            // write received message to stdout
            ssize_t w = write(STDOUT_FILENO, recvbuf, n);
            if (w != n) {
                // ignore partial write errors
            }
        }
    }

    close(sockfd);
    return 0;
}

