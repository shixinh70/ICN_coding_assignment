#include "header.h"
#define CORRUPT_DETECT 1

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
    Dport = SERVER_PORT;
    initS(&sendS,sPort,Dport);
    replyS(&sendS,currentSeg,currentAck,SYN);
    sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client",0);
    recvpacket(socket_fd,i_buffer,sizeof(i_buffer),&recvS,"client");
    currentSeg = recvS.l4info.AckNum;
    currentAck = recvS.l4info.SeqNum+1;
    replyS(&sendS,currentSeg,currentAck,ACK);
    sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client",0);
    /*--------3handshake---------*/
    printf("----------------------------Receive data!-----------------------------\n");
    /*-------------------receive data-------------------------------------------------------*/
    Segment last_recv_packet = recvS;
    int base = sendS.l4info.AckNum;
    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }
    while(1){
        ssize_t byterecv = recvpacket(socket_fd,i_buffer,sizeof(i_buffer),&recvS,"client");
        if(CORRUPT_DETECT ? !packet_corrupt(i_buffer,byterecv,"client") : 1){
            if(recvS.l4info.SeqNum == sendS.l4info.AckNum){
                last_recv_packet = recvS;
                fwrite(recvS.payload, 1, recvS.p_len, file);
                currentAck = last_recv_packet.l4info.SeqNum + last_recv_packet.p_len;
                
                replyS(&sendS,currentSeg,currentAck,ACK);
                sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client",0);
                
                sleep(0);
            }
            else{
                printf("Rdt client: accumulate ack ,send last ack packet!\n");
                sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client",0);
                sleep(0);
            }
            
        }
        else{
            printf("Rdt client: Dropped corrupt pakcet\n");
            sendpacket(socket_fd,o_buffer,sizeof(o_buffer),&sendS,"client",0);
            sleep(0);
        }
    }
    
}