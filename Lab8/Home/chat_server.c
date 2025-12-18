/* tcp_chat_server.c
   Simple TCP chatroom server using select().
   Accepts multiple clients and broadcasts each client's message to all other clients.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <signal.h>

#define PORT 7000
#define MAX_CLIENTS  FD_SETSIZE
#define BUF_SIZE 1024

int listen_fd = -1;

void handle_sigint(int sig) {
    if (listen_fd != -1) close(listen_fd);
    fprintf(stderr, "\nServer shutting down (SIGINT).\n");
    exit(0);
}

int main(void) {
    signal(SIGINT, handle_sigint);

    int clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; ++i) clients[i] = -1;

    struct sockaddr_in serv_addr;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) { perror("socket"); exit(EXIT_FAILURE); }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if (bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind"); close(listen_fd); exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 10) == -1) {
        perror("listen"); close(listen_fd); exit(EXIT_FAILURE);
    }

    printf("Chatroom server listening on port %d\n", PORT);

    fd_set allset, rset;
    FD_ZERO(&allset);
    FD_SET(listen_fd, &allset);
    int maxfd = listen_fd;

    while (1) {
        rset = allset; // copy
        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0) {
            if (errno == EINTR) continue;
            perror("select"); break;
        }

        // New connection?
        if (FD_ISSET(listen_fd, &rset)) {
            int connfd = accept(listen_fd, NULL, NULL);
            if (connfd == -1) {
                perror("accept");
            } else {
                int i;
                for (i = 0; i < MAX_CLIENTS; ++i) {
                    if (clients[i] < 0) {
                        clients[i] = connfd;
                        break;
                    }
                }
                if (i == MAX_CLIENTS) {
                    fprintf(stderr, "Too many clients, rejecting connection.\n");
                    close(connfd);
                } else {
                    FD_SET(connfd, &allset);
                    if (connfd > maxfd) maxfd = connfd;
                    printf("New client connected: fd=%d\n", connfd);
                }
            }
            if (--nready <= 0) continue;
        }

        // Check clients for data
        for (int i = 0; i < MAX_CLIENTS && nready > 0; ++i) {
            int fd = clients[i];
            if (fd < 0) continue;
            if (FD_ISSET(fd, &rset)) {
                char buf[BUF_SIZE];
                ssize_t n = read(fd, buf, sizeof(buf));
                if (n <= 0) {
                    // connection closed or error
                    if (n == 0) {
                        printf("Client fd=%d disconnected.\n", fd);
                    } else {
                        perror("read");
                    }
                    close(fd);
                    FD_CLR(fd, &allset);
                    clients[i] = -1;
                } else {
                    // broadcast to all other clients
                    for (int j = 0; j < MAX_CLIENTS; ++j) {
                        if (clients[j] != -1 && clients[j] != fd) {
                            ssize_t w = write(clients[j], buf, n);
                            if (w != n) {
                                // ignore partial write errors for simplicity
                            }
                        }
                    }
                }
                --nready;
            }
        }
    }

    close(listen_fd);
    return 0;
}

