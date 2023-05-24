#include "header.h"
int main(){
    srand(getpid());
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
    /*--------3handshake---------*/
    char o_buffer[20], i_buffer[1020];
    uint32_t currentSeg, currentAck;
    uint16_t sPort,Dport;
    Segment sendS,recvS;

    currentSeg = rand();
    currentAck = 0;
    sPort = (rand()%65535)+1;
    printf("%d",sPort);
    Dport = SERVER_PORT;
    initS(&sendS,sPort,Dport);
    replyS(&sendS,currentSeg,currentAck,SYN);
    _headermaker(&sendS);
    memcpy(o_buffer,&sendS,sizeof(o_buffer));
    send(socket_fd,o_buffer,sizeof(o_buffer),0);
    
}