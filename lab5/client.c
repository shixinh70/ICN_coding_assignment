#include "header.h"
int corrupt(double probability) {
    double randomNum = (double)rand() / RAND_MAX;

    if (randomNum <= probability) {
        return 1;
    } 
    else {
        return 0;
    }
}
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
    //printf("%d",sPort);
    Dport = SERVER_PORT;
    initS(&sendS,sPort,Dport);
    replyS(&sendS,currentSeg,currentAck,SYN);
    sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client");

    recvpacket(socket_fd,i_buffer,sizeof(i_buffer),&recvS,"client");
    currentSeg = recvS.l4info.AckNum+1;
    currentAck = recvS.l4info.SeqNum+1;
    replyS(&sendS,currentSeg,currentAck,ACK);
    sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client");
    /*--------3handshake---------*/
    printf("------------------\n");
    /*-------------------receive data-------------------------------------------------------*/
    Segment last_recv_packet = recvS;
    int base = sendS.l4info.AckNum;
    while(1){
        ssize_t byterecv = recvpacket(socket_fd,i_buffer,sizeof(i_buffer),&recvS,"client");
        printf("%d %d\n",recvS.l4info.SeqNum,sendS.l4info.AckNum);
        if((!corrupt(0.3))){
            if(recvS.l4info.SeqNum == sendS.l4info.AckNum){
                last_recv_packet = recvS;
                currentAck = last_recv_packet.l4info.SeqNum + byterecv - 20;
                replyS(&sendS,currentSeg,currentAck,ACK);
                sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client");
                sleep(0);
            }
            else{
                printf("Rdt client: Not accmualate ack!\n");
                sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client");
                sleep(0);
            }
            
        }
        else{
            printf("Rdt client: Receive corrupt pakcet\n");
            sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client");
            sleep(0);
        }
    }



    
    
    // _headermaker(&sendS);
    // memcpy(o_buffer,&sendS,sizeof(o_buffer));
    // send(socket_fd,o_buffer,sizeof(o_buffer),0);
    
}