#include "header.h"
#define PORT 45525



void server_accept(int fd){
    Segment sendS,recvS;
    char i_buffer[256],o_buffer[256];
    printf("App server: Waiting for SYN packet!\n");
    recv(fd,i_buffer,sizeof(i_buffer),0);
    printheader(i_buffer);
    parse_packet(i_buffer,&recvS);
    memset(i_buffer,0,sizeof(i_buffer));
    if(recvS.l4info.DesPort!=45525){
        printf("App server:%d Port is closed\n",recvS.l4info.DesPort);
        exit(1);    
    }
    if(recvS.l4info.Flag!=0x02){
        printf("App server: Not a SYN Packet\n");
        exit(1);
    }
    int c_isn = recvS.l4info.SeqNum;
    memcpy(&sendS,&recvS,sizeof(recvS));
    sendS.l4info.SeqNum = rand();
    sendS.l4info.AckNum = c_isn + 1;
    sendS.l4info.Flag = 0x012;
    uint16_t client_port = recvS.l4info.SourcePort;
    sendS.l4info.SourcePort = 45525;
    sendS.l4info.DesPort = client_port;
    _headermaker(&sendS);
    memcpy(o_buffer,sendS.header,sizeof(sendS.header));
    printf("App server: Send SYN ACK packet!\n");

    send(fd,o_buffer,20,0);
    memset(o_buffer,0,sizeof(o_buffer));
    printf("App server: Waiting for ACK packet!\n");
    recv(fd,i_buffer,sizeof(i_buffer),0);
    parse_packet(i_buffer,&recvS);
    memset(i_buffer,0,sizeof(i_buffer));
    if(recvS.l4info.DesPort!=45525){
        printf("App server: %d Port is closed\n",recvS.l4info.DesPort);
        exit(1);    
    }
    if(recvS.l4info.Flag!=0x10){
        printf("App server: Not a ACK Packet\n");
        exit(1);
    }
    printf("App server: Accept connection from port %d\n",client_port);
}
int udt_server_connect(int port){
    int socket_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (socket_fd < 0){
        printf("Udt server: Fail to create a socket.");
    }

    // Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(port)
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
    int fd = udt_server_connect(PORT);
    server_accept(fd);
    
    
    
}