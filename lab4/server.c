#include "header.h"


typedef struct packet{
    char buffer[1020];
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
    uint32_t c_isn = recvS.l4info.SeqNum;
    uint32_t s_isn = rand();
    
    memcpy(&sendS,&recvS,sizeof(recvS));
    sendS.l4info.SeqNum = s_isn;
    sendS.l4info.AckNum = c_isn + 1;
    sendS.l4info.Flag = 0x012;
    uint16_t client_port = recvS.l4info.SourcePort;
    sendS.l4info.SourcePort = server_port;
    sendS.l4info.DesPort = client_port;
    _headermaker(&sendS);
    memcpy(o_buffer,sendS.header,sizeof(sendS.header));
    printf("App server: Send SYN ACK packet!\n");
    printsegment(&sendS);
    send(fd,o_buffer,20,0);
    
    memset(o_buffer,0,sizeof(o_buffer));
    printf("App server: Waiting for ACK packet!\n");
    recv(fd,i_buffer,sizeof(i_buffer),0);
    parse_packet(i_buffer,&recvS);
    
    memset(i_buffer,0,sizeof(i_buffer));
    if(recvS.l4info.DesPort!=server_port){
        printf("App server: %d Port is closed\n",recvS.l4info.DesPort);
        exit(1);    
    }
    if(recvS.l4info.Flag!=0x10){
        printf("App server: Not a ACK Packet\n");
        exit(1);
    }
    printsegment(&recvS);
    if(recvS.l4info.AckNum != s_isn +1 || recvS.l4info.SeqNum!= c_isn+1){
        printf("App server: Wrong Seq/Ack num\n");
        exit(1);
    }
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
        
        while(bytesRead>1){
            for(int i=0;i<10;i++){
                bytesRead = fread((packetbuffer+i)->buffer+20, 1, 1020-20, file);
                if(bytesRead<1) break;
                sendS.l4info.SeqNum = current_seq;
                sendS.l4info.AckNum = current_ack;
                sendS.l4info.Flag = 0X18;
                _headermaker(&sendS);
                memcpy((packetbuffer+i)->buffer,sendS.header,20);
                send(fd,(packetbuffer+i)->buffer,bytesRead+20,0);
                printf("send: seg %d, ack %d\n",sendS.l4info.SeqNum,sendS.l4info.AckNum);
                current_seq += bytesRead;
            }
            recv(fd,i_buffer,20,0);
            parse_packet(i_buffer,&recvS);

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
// void transmit_data(int client_fd){
//     Packet packetbuffer[10];
//     char o_buffer[1020];
//     FILE* file = fopen("image.jpg", "rb");
//         if (file == NULL) {
//             perror("fail to open");
//             exit(1);
//         }
//         size_t bytesRead = 2;
//         while(bytesRead>1){
//             for(int i=0;i<10;i++){
//                 bytesRead = fread((packetbuffer+i)->buffer+20, 1, 1020-20, file);
//                 if(bytesRead>1) break;
//                 memcpy((packetbuffer+i)->buffer,header,20);
//             }
//         }
        
//         while ((bytesRead = fread(o_buffer, 1, sizeof(o_buffer), file)) > 0) {
//             if (send(client_fd, o_buffer, bytesRead, 0) < 0) {
//                 perror("Fail to send");        
//                 exit(1);
//             }
//             memset(i_buffer,0,sizeof(i_buffer));
//             recv(client_fd,i_buffer,sizeof(i_buffer),0);
//             printf("send %s\n",i_buffer);
            
//         }
//         fclose(file);
// }
int main(){
    int fd = udt_server_connect();
    server_accept(fd,45525);
    

    
}