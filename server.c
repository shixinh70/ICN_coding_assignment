#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "header.h"

int main(int argc , char *argv[]){
    srand(time(NULL));
    
    int socket_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (socket_fd < 0){
        printf("Fail to create a socket.");
    }

    // Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(45525)
    };

    //Bind socket to the address.
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    
    //Listening the socket.
    if (listen(socket_fd, 5) == -1) {
        printf("socket %d listen failed!\n", socket_fd);
        close(socket_fd);
        exit(0);
    }
    char o_buffer[1024];
    int client_fd;
    struct sockaddr_in clientAddr;
    int client_len = sizeof(clientAddr);

    //Accept the connect request.
    while(1){
        client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&client_len);
        strcpy(o_buffer,"Hi, I'm server 111065540");
        send(client_fd,o_buffer,sizeof(o_buffer),0);
        memset(o_buffer,0,sizeof(o_buffer));
        serverfunction(client_fd);
    }
    
    ////////////////////////////////////////////////////////////
    //                   TASK 1(Server)                       //
    ////////////////////////////////////////////////////////////
    // TODO: create a TCP socket bind to port 45525.          //
    // TODO: Listen the TCP socket.                           //
    // TODO: Accept the connect and get the Client socket     //
    //       file descriptor.                                 //
    // TODO: Send 1 message "Hi, I'm server {Your_student_ID}"//
    //       to client.                                       //
    // Then go finish the client.c TASK 1                     //
    ////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    //                   TASK 2,3(Server)                     //
    ////////////////////////////////////////////////////////////
    // TODO: Put the client socket fd into serverfuntion()    //
    //                                                        //
    // Example:                                               //
    //           serverfunction(clientsocketfd);              //
    //                                                        //
    // Then go finish the client.c TASK2,3                    //
    ////////////////////////////////////////////////////////////
}
