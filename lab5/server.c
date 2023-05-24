#include"header.h"
#define SERVER_PORT 45525
#define SYN 0X2
#define ACK 0X10 
#define SYNACK 0X12 
int main(){    
    srand(getpid());

    int socket_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (socket_fd < 0){
        printf("Server: Fail to create a socket.");
    }

    // Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(SERVER_PORT)
    };

    //Bind socket to the address.
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Server: Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    
    //Listening the socket.
    if (listen(socket_fd, 5) == -1) {
        printf("Server: socket %d listen failed!\n", socket_fd);
        close(socket_fd);
        exit(0);
    }
    int client_fd;
    struct sockaddr_in clientAddr;
    int client_len = sizeof(clientAddr);

    //Accept the connect request.
    char ipAddr[INET_ADDRSTRLEN];

    while(1){
        printf("Server: waiting for client connect\n");
        client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&client_len);
        inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddr, INET_ADDRSTRLEN);//Get client ipaddr(string);
        printf("Server: accept client from %s:%d\n",ipAddr,ntohs(clientAddr.sin_port));
        /*---------------Rdt Server----------------*/
        char i_buffer[20];
        char o_buffer[1020];
        Segment recvS,sendS;
        uint32_t currentSeg, currentAck;
        uint16_t dPort;

        printf("Rdt server: Hello, This is Rdt server on 127.0.0.1:45525\n");
        printf("Rdt server: Waiting for the client to connect\n");
        /*--------------3handshake-----------------*/
        recv(client_fd,i_buffer,sizeof(i_buffer),0);
        parse_packet(i_buffer,&recvS);
        printf("Rdt server: Receive packet\n");

        if(recvS.l4info.DesPort!=SERVER_PORT||recvS.l4info.Flag!=SYN){
            if(recvS.l4info.DesPort!=SERVER_PORT) printf("Rdt server: Port %u is closed!\n",recvS.l4info.DesPort);
            else printf("Rdt server: Not a SYN packet(0x2)\n");
            close(client_fd);
            continue;
        }
        else{
            printf("Rdt server: handshaking with client from 127.0.0.1:%u\n", recvS.l4info.SourcePort);
            dPort = recvS.l4info.SourcePort;
            currentSeg = rand();
            currentAck = recvS.l4info.SeqNum+1;
            initS(&sendS,SERVER_PORT,dPort);
            replyS(&sendS,currentSeg,currentAck,SYNACK);
            // _headermaker(&sendS);
            // printf("Rdt server: Send SYN ACK packet to port %u\n", sendS.l4info.DesPort);
            // send(client_fd,o_buffer,sizeof(o_buffer),0);
            sendpacket(client_fd,o_buffer,&sendS);
            while(1);
        } 
        /*-----------------------------------------*/


        printf("Server: close\n");
        close(client_fd);
    }

}