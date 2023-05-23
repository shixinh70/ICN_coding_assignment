#include "header.h"

typedef struct packet{
    char buffer[1020];
    uint32_t ack;
}Packet;
void server_accept(int fd, uint16_t server_port){
    srand(getpid());
    Segment sendS,recvS;
    char i_buffer[1020],o_buffer[1020];


    printf("App server: Waiting for SYN packet!\n");
    recv(fd,i_buffer,sizeof(i_buffer),0);
    parse_packet(i_buffer,&recvS);
    printsegment(&recvS);
    memset(i_buffer,0,sizeof(i_buffer));
    if(recvS.l4info.DesPort!=server_port){
        printf("App server:%d Port is closed\n",recvS.l4info.DesPort);
        exit(1);    
    }
    if(recvS.l4info.Flag!=0x02){
        printf("App server: Not a SYN Packet\n");
        exit(1);
    }
    printf("App server: Received SYN packet from port %d!\n",recvS.l4info.SourcePort);
    uint32_t c_isn = recvS.l4info.SeqNum;
    uint32_t s_isn = rand();
    uint16_t client_port = recvS.l4info.SourcePort;

    memcpy(&sendS,&recvS,sizeof(recvS));
    sendS.l4info.SeqNum = s_isn;
    sendS.l4info.AckNum = c_isn + 1;
    sendS.l4info.Flag = 0x012;
    sendS.l4info.SourcePort = server_port;
    sendS.l4info.DesPort = client_port;
    _headermaker(&sendS);
    memcpy(o_buffer,sendS.header,sizeof(sendS.header));
    printf("App server: Send SYN ACK packet!\n");

    send(fd,o_buffer,20,0);
    memset(o_buffer,0,sizeof(o_buffer));

    printf("App server: Waiting for ACK packet!\n");
    recv(fd,i_buffer,sizeof(i_buffer),0);
    parse_packet(i_buffer,&recvS);
    printsegment(&recvS);
    memset(i_buffer,0,sizeof(i_buffer));
    if(recvS.l4info.DesPort!=server_port){
        printf("App server: %d Port is closed\n",recvS.l4info.DesPort);
        exit(1);    
    }
    if(recvS.l4info.SourcePort!=client_port){
        printf("App server: Unknown source port %d\n",recvS.l4info.SourcePort);
        exit(1);   
    }
    if(recvS.l4info.Flag!=0x10){
        printf("App server: Not a ACK Packet\n");
        exit(1);
    }    
    
    if(recvS.l4info.AckNum != s_isn +1 || recvS.l4info.SeqNum!= c_isn+1){
        printf("App server: Wrong Seq/Ack num\n");
        exit(1);
    }
    printf("App server: Received ACK packet from port %d!\n",recvS.l4info.SourcePort);
    printf("App server: Accept connection from port %d\n",client_port);
    printf("App server: Start to trnasmit image\n");
    ///////////////transmit data///////////
    uint32_t current_seq = s_isn+1, current_ack = c_isn+1;
    Packet packetbuffer[10];
    FILE* file = fopen("image.jpg", "rb");
        if (file == NULL) {
            perror("fail to open");
            exit(1);
        }
        size_t bytesRead = 2;
        int count = 0;
        while(bytesRead>1){
            count = 0;
            for(int i=0;i<10;i++){
                bytesRead = fread((packetbuffer+i)->buffer+20, 1, 1020-20, file);
                if(bytesRead<1) break;
                count ++;
                sendS.l4info.SeqNum = current_seq;
                sendS.l4info.AckNum = current_ack;
                sendS.l4info.Flag = 0X18;
                _headermaker(&sendS);
                memcpy((packetbuffer+i)->buffer,sendS.header,20);
                (packetbuffer+i)->ack = sendS.l4info.SeqNum + bytesRead + 1;
                send(fd,(packetbuffer+i)->buffer,bytesRead+20,0);
                printf("send packet %d: seg %u, ack %u\n",(sendS.l4info.SeqNum-s_isn-1)/1000,sendS.l4info.SeqNum,sendS.l4info.AckNum);
                current_seq += bytesRead;
            }
            
            int base;
            uint32_t recv_acknum;
            for(int i=0;i<count;i++){
                recv(fd,i_buffer,20,0);
                parse_packet(i_buffer,&recvS);
                printf("recv: seg %u, ack %u\n",recvS.l4info.SeqNum,recvS.l4info.AckNum);
                memcpy(&recv_acknum,i_buffer+8,4);    
                recv_acknum = ntohl(recv_acknum);
                for(int i=0;i<count;i++){
                    if((packetbuffer+i)->ack==recv_acknum){
                        base = i;
                        break;
                    }
                }
                printf("Acc ack = %d\n",base);
            }
            while(base!=count-1){
                sleep(5);
                printf("Time out!, Resend packet!\n");
                send(fd,(packetbuffer+base+1)->buffer,bytesRead+20,0);
                parse_packet((char*)(packetbuffer+base+1),&sendS);
                printf("send packet %d: seg %u, ack %u\n",(sendS.l4info.SeqNum-s_isn-1)/1000,sendS.l4info.SeqNum,sendS.l4info.AckNum);
                
                recv(fd,i_buffer,20,0);
                memcpy(&recv_acknum,i_buffer+8,4);    
                recv_acknum = ntohl(recv_acknum);
                for(int i=0;i<count;i++){
                    if((packetbuffer+i)->ack==recv_acknum){
                        base++;
                        break;
                    }
                }
            }    
        }



}
int udt_server_connect(){
    int socket_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (socket_fd < 0){
        printf("Udt server: Fail to create a socket.");
    }

    // Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(45525)
    };

    //Bind socket to the address.
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Udt server: Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    
    //Listening the socket.
    if (listen(socket_fd, 5) == -1) {
        printf("socket %d listen failed!\n", socket_fd);
        close(socket_fd);
        exit(0);
    }

    int client_fd;
    struct sockaddr_in clientAddr;
    int client_len = sizeof(clientAddr);

    //Accept the connect request.
    
    client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&client_len);
    printf("Udt server: accept connect\n");
    return client_fd;
    
}

int main(){
    int fd = udt_server_connect();
    server_accept(fd,45525);
    

    
}