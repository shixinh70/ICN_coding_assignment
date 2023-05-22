#include<stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
void _checksummaker(Segment* s){

    char temp[32] ={0};
    uint32_t buffer = 0;
    memcpy(temp,s->header,20);
    memcpy(temp+20,s->pseudoheader,12);
    uint16_t* p = (uint16_t*)temp;
    for(int i =0;i<16;i++){
        buffer += *(p+i);
    }
    buffer = (~((buffer&0xffff) + (buffer>>16)))&0xffff;
    memcpy(s->header+16,&buffer,2);
}
void _psuedoheadmaker(Segment* s){
    
    uint32_t temp[3] = {0};
    temp[0] = inet_addr(s->l3info.SourceIpv4);
    temp[1] = inet_addr(s->l3info.DesIpv4);
    temp[2] = htonl((s->l4info.HeaderLen*4)+(s->l3info.protocol<<16) + 0);
    for(int i=0;i<3;i++){//htonl 把高記憶體位置的東西和低記憶體位置的東西互換 (只要照原本的高低位填，在互換就會得到要的header)
        memcpy(s->pseudoheader+4*i,temp+i,4);
    }
}
void _tcpheadermaker(Segment* s){
    
    uint32_t temp[5] = {0};//把array中最高記憶體位置的東西抓到最低記憶體位置(header中高記憶體位置為(Source port address)，所以要填高記憶體位置)
    temp[0] = 0;
    temp[1] = s->l4info.WindowSize + (s->l4info.Flag<<16) +(s->l4info.HeaderLen<<28);
    temp[2] = s->l4info.AckNum;
    temp[3] = s->l4info.SeqNum;
    temp[4] = s->l4info.DesPort + (s->l4info.SourcePort << 16);
    uint8_t *ph = (uint8_t*)temp;
    for(int i=0;i<20;i++){
        s->header[i] = *(ph+(20-i-1));
    }
}
void _headermaker(Segment* s){
    
    _tcpheadermaker(s);
    _psuedoheadmaker(s);
    _checksummaker(s);
    

}
typedef struct l4info{
    uint32_t AckNum,SeqNum,SourcePort,DesPort,Flag,HeaderLen,WindowSize,CheckSum;
}L4info;

typedef struct l3info{
    char SourceIpv4[16], DesIpv4[16];
    uint32_t protocol;
}L3info;

typedef struct Segment{
    char header[20];
    char pseudoheader[12];
    L3info l3info;
    L4info l4info;
}Segment;
typedef enum state{
    LISTEN,
    WAIT_SYNACK,
    CONNECT
}State;
typedef struct udt{

    int (*udt_connect)();
    ssize_t (*udt_send)();
    ssize_t (*udt_recv)();
}Udt;  
typedef struct userinfo{
    char s_ip[20],d_ip[20];
    uint16_t id,s_port,d_port;

}Userinfo;
typedef struct appinfo{
    char ip[20];
    uint16_t port;
}Appinfo;
int udt_connect(int port){
            //Create socket.
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Udt: Create socket fail!\n");
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
        printf("Udt :Connect failed!\n");
        close(socket_fd);
        exit(0);
    }
    printf("Udt :Connect Successfully!\n");
}
void parse_packet(char* recvbuffer,Segment* recvSegment){
    strcpy(recvSegment->l3info.DesIpv4,"127.0.0.1");
    strcpy(recvSegment->l3info.SourceIpv4,"127.0.0.1");
    recvSegment->l3info.protocol = 6;
    char header[20];
    memcpy(header,recvbuffer,20);
    recvSegment->l4info.SourcePort = header[0]<<8+header[1];
    recvSegment->l4info.DesPort = header[2]<<8+header[3];
    recvSegment->l4info.SeqNum = header[4]<<24+header[5]<<16+header[6]<<8+header[7];
    recvSegment->l4info.AckNum = header[8]<<24+header[9]<<16+header[10]<<8+header[11];
    recvSegment->l4info.HeaderLen = header[12]>>4;
    recvSegment->l4info.Flag =header[13];
    recvSegment->l4info.WindowSize = header[14]<<8+header[15];
    recvSegment->l4info.CheckSum = header[16]<<8+header[17];

}

void reply_packet(State* state, Segment *recvSegment){
    _headermaker(recvSegment);
    int compchecksum = (recvSegment->header[16])<<8 + recvSegment->header[17];
    if(recvSegment->l4info.CheckSum!=compchecksum){
        printf("Recieved corrupted packet\n");
    }
    if (state == LISTEN){

    }
    else if (state == WAIT_SYNACK){}
    else if (state == CONNECT){}

    
}

// void parse_packet(char buffer, Info* user_info){
//     char header[20] = {0};
//     memcpy(header, buffer, 20);

//     if(user_info->state = LISTEN){
//         user_info->id = rand();
//         strcpy(user_info->ip,"127.0.0.1");
//         user_info->port = 
//     }
//     else if (user_info->state = WAIT_SYNACK){}
//     else if (user_info->state = CONNECT){}
    
// }



ssize_t udt_send();
ssize_t udt_recv();
void app_server(int fd){
    char buffer[256] = {0};
    Appinfo app_info;
    Segment recvSegment;
    Userinfo user_info;
    State state = LISTEN;
    int byterecv;
    
    //wating for handshake
    while(1){
        if(state = LISTEN){
            printf("App Server: Listening on %s:%d\n",app_info.ip,app_info.port);

        }
        else if (state = WAIT_SYNACK){
            printf("App Server: Waiting for SYN ACK packet\n");

        }
        else if (state = CONNECT){
            printf("App Server: Successfuly Connect\n");
            break;
        }
        byterecv = recv(fd,buffer,sizeof(buffer),0);
        printf("App Server: Receive Packet, size = %d bytes!\n",byterecv);
        parse_packet(&state,buffer,&recvSegment);
        reply_packet(&state,&recvSegment);
        memset(buffer,0,sizeof(buffer));

    }

    //sending img
    //recv ack

}


int main(){
    int client_fd = udt_connect(45525);
    app_server(client_fd);

}