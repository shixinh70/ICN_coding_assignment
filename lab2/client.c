#include "header.h"
// void myheadercreater(Segment* s){
//     char psu_header[12] = {0};
//     uint32_t tmp;
//     tmp =inet_addr(s->l3info.DesIpv4);
//     memcpy(psu_header,&tmp,4);
//     tmp =inet_addr(s->l3info.SourceIpv4);
//     memcpy(psu_header+4,&tmp,4);
//     tmp = (6<<16) + s->l4info.HeaderLen*4;
//     tmp = htonl(tmp);
//     memcpy(psu_header+8,&tmp,4);

//     char tcpheader[20] = {0};
//     tmp = (uint32_t)(s->l4info.SourcePort<<16) + s->l4info.DesPort;
//     tmp = htonl(tmp);
//     memcpy(tcpheader,&tmp,4);
//     tmp = (uint32_t)s->l4info.SeqNum;
//     tmp = htonl(tmp);
//     memcpy(tcpheader+4,&tmp,4);
//     tmp = (uint32_t)s->l4info.AckNum;
//     tmp = htonl(tmp);
//     memcpy(tcpheader+8,&tmp,4);
//     tmp = (uint32_t)(s->l4info.HeaderLen <<28) + (0x10 <<16) + s->l4info.WindowSize;
//     tmp = htonl(tmp);
//     memcpy(tcpheader+12,&tmp,4);

//     char header[32];
//     memcpy(header,psu_header,12);
//     memcpy(header+12,tcpheader,20);

//     uint16_t* p = (uint16_t*)header;
//     uint32_t sum = 0;
//     for(int i=0;i<16;i++){
//         sum+= *(p+i);
//     }
//     sum = ~((sum>>16) + (sum & 0xffff));
//     memcpy(tcpheader+16,&sum,2);
//     memcpy(s->header,tcpheader,20);
// }

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
        .sin_family = PF_INET,
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
    
    char buffer[1024] = {0};
    recv(socket_fd,buffer,sizeof(buffer),0);
    printf("%s\n",buffer);
    memset(buffer,0,sizeof(buffer));

    Segment s;
    receivedata(socket_fd,&s);
   
    sendheader(socket_fd,s.header);
    close(socket_fd);
}
