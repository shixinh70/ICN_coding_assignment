#include "header.h"


int main(int argc , char *argv[])
{   
    //Create socket.
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Create socket fail!\n");
        return -1;
    }

    //Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = PF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(45525)
    };
    int server_len = sizeof(serverAddr);

    //Connect to server's socket.
    if (connect(socket_fd, (struct sockaddr *)&serverAddr, server_len) == -1) {
        printf("Connect server failed!\n");
        close(socket_fd);
        exit(0);
    }
    
    char buffer[1024] = {0};
    recv(socket_fd,buffer,sizeof(buffer),0);
    printf("%s\n",buffer);
    memset(buffer,0,sizeof(buffer));

    Segment s;
    receivedata(socket_fd,&s);
   
    sendheader(socket_fd,s.header);
    close(socket_fd);
}