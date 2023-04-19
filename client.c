#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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
        .sin_family = AF_INET,
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
    
    char i_buffer[1024] = {0};
    recv(socket_fd,i_buffer,sizeof(i_buffer),0);    
    printf("%s\n",i_buffer);


    //////////////////////////////////////////////////////////
    //                   TASK1(Client)                      //  
    //////////////////////////////////////////////////////////
    // TODO: create a socket and connet to server.          //
    //       (server's IP address = 127.0.0.1)              //
    //       (server's Port number = 45525)                 //
    //                                                      //
    // TODO: Receive 1 message from server and print it out.//
    //       (The message you sent from server)             //
    //////////////////////////////////////////////////////////
    


    ///////////////////////////////////////////////////////////
    //                   TASK2,3(Client)                     //
    ///////////////////////////////////////////////////////////
    // TODO: Instantiate a Segment                           //
    // TODO: Send your socket_fd and the instance of Segment //
    //       into the receivedata().                         //
    // TODO: Write your own function to create header.       //
    //       (All information is in the Segment instance.    //
    // TODO: Use sendheader(char* header) to send the header //
    //       to server.                                      //
    //                                                       //
    // Example:                                              //
    //                                                       //
    //       Segment s;                                      //
    //       receivedata(sock_fd,&s);                        //
    //       myheadercreater(&s);  //your own function       //
    //       sendheader(sock_fd,s.header);                   // 
    //                                                       //
    //                                                       //
    // Then you will see the result.                         //  
    ///////////////////////////////////////////////////////////
    
    Segment s;
    receivedata(socket_fd,&s);
     s.header[12] = 0x50;
     s.header[13] = 0x10;
    
    sendheader(socket_fd,s.header);
    
    close(socket_fd);
}