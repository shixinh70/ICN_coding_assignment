#include "header.h"

int udt_client_connect(int port){

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Udt client: Create socket fail!\n");
        return -1;
    }

    //Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(port)
    };
    int server_len = sizeof(serverAddr);

    //Connect to server's socket.
    if (connect(socket_fd, (struct sockaddr *)&serverAddr, server_len) == -1) {
        printf("Udt client: Connect server failed!\n");
        close(socket_fd);
        exit(0);
    }
    printf("Udt client: Connect server successfully\n");
    return socket_fd;
}
void app_client_connect(int fd, int port){
    Segment sendS,recvS;
    char o_buffer[256],i_buffer[256];
    int client_port = myrand(1,65535);
    strcpy(sendS.l3info.SourceIpv4,"127.0.0.1");
    strcpy(sendS.l3info.DesIpv4,"127.0.0.1");
    sendS.l3info.protocol = 6;
    sendS.l4info.SeqNum = rand();
    sendS.l4info.AckNum = 0;
    sendS.l4info.Flag = 0x002;
    sendS.l4info.HeaderLen = 5;
    sendS.l4info.SourcePort = client_port;
    sendS.l4info.DesPort = port;
    sendS.l4info.WindowSize = 65535;
    _headermaker(&sendS);
    
    memcpy(o_buffer,sendS.header,20); 
    
    send(fd,o_buffer,20,0);
    printheader(o_buffer);
    memset(o_buffer,0,sizeof(o_buffer));
    recv(fd,i_buffer,sizeof(i_buffer),0);
    parse_packet(i_buffer,&sendS);

    int s_isn = recvS.l4info.SeqNum;
    memcpy(&sendS,&recvS,sizeof(recvS));
    sendS.l4info.SeqNum = recvS.l4info.AckNum +1;
    sendS.l4info.AckNum = s_isn + 1;
    sendS.l4info.Flag = 0x010;
    sendS.l4info.SourcePort = client_port;
    sendS.l4info.DesPort = 45525;
    _headermaker(&sendS);
    
    memcpy(o_buffer,sendS.header,20); 
    //send(fd,o_buffer,20,0);
    memset(o_buffer,0,sizeof(o_buffer));
    

}

int main(){
    int fd = udt_client_connect(45525);
    app_client_connect(fd,45525);
    //transmit_data();
    while(1);
    
}