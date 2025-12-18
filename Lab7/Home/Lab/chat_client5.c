#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define SV_SOCK_PATH "/tmp/chat_server"
#define BUF 1024

int main() {
    int sfd;
    struct sockaddr_un addr;
    char buf[BUF];
    fd_set r;
    ssize_t n;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr,0,sizeof(addr));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path,SV_SOCK_PATH,sizeof(addr.sun_path)-1);
    connect(sfd,(struct sockaddr*)&addr,sizeof(addr));

    printf("Connected to chat server. Type messages (Ctrl+D to quit):\n");
    fflush(stdout);

    while(1){
        FD_ZERO(&r);
        FD_SET(STDIN_FILENO,&r);
        FD_SET(sfd,&r);

        if(select(sfd+1,&r,NULL,NULL,NULL)<0) exit(1);

        if(FD_ISSET(STDIN_FILENO,&r)){
            if(fgets(buf,BUF,stdin)!=NULL) write(sfd,buf,strlen(buf));
            else break;
        }

        if(FD_ISSET(sfd,&r)){
            n=read(sfd,buf,BUF-1);
            if(n<=0){printf("Server closed connection.\n"); break;}
            buf[n]='\0';
            printf("%s",buf); fflush(stdout);
        }
    }

    close(sfd);
}
