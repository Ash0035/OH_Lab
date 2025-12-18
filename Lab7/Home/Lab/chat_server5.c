#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define SV_SOCK_PATH "/tmp/chat_server"
#define BUF 1024
#define MAX 10

int main() {
    int sfd, clients[MAX] = {-1}, maxfd, i, cfd;
    struct sockaddr_un addr;
    char buf[BUF];
    ssize_t n;
    fd_set all, r;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SV_SOCK_PATH);
    memset(&addr,0,sizeof(addr));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH,sizeof(addr.sun_path)-1);
    bind(sfd,(struct sockaddr*)&addr,sizeof(addr));
    listen(sfd, MAX);

    FD_ZERO(&all); FD_SET(sfd,&all); maxfd=sfd;

    while(1){
        r=all;
        if(select(maxfd+1,&r,NULL,NULL,NULL)<0) exit(1);

        if(FD_ISSET(sfd,&r)){
            cfd=accept(sfd,NULL,NULL);
            for(i=0;i<MAX;i++) if(clients[i]<0){clients[i]=cfd; break;}
            FD_SET(cfd,&all); if(cfd>maxfd) maxfd=cfd;
        }

        for(i=0;i<MAX;i++){
            int fd=clients[i]; if(fd<0) continue;
            if(FD_ISSET(fd,&r)){
                n=read(fd,buf,BUF-1);
                if(n<=0){close(fd); FD_CLR(fd,&all); clients[i]=-1;}
                else{buf[n]='\0'; for(int j=0;j<MAX;j++) if(clients[j]>=0&&clients[j]!=fd) write(clients[j],buf,n);}
            }
        }
    }
}
