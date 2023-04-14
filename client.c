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
    // TODO: Create a Segment                                //
    // TODO: Put your socket fd into the receivedata()       //
    // TODO: Write your own function to create header        //
    //       (All information is in the Segment instance.    //
    // TODO: Use sendheader(char* header) to send the header //
    //       to server.                                      //
    //                                                       //
    // Example:                                              //
    //                                                       //
    //       Segment s;                                      //
    //       receivedata(sockfd,&s);                         //
    //       myheadercreater(&s);                            //
    //       sendheader(sockfd,s.header)                     // 
    //                                                       //
    //                                                       //
    // Then you will see the result.                         //  
    ///////////////////////////////////////////////////////////
    //_headermaker(&s,"asiagodtone");我寫的headermaker用法
}