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
int udt_client_connect(){

    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Udt client: Create socket fail!\n");
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
        printf("Udt client: Connect server failed!\n");
        close(socket_fd);
        exit(0);
    }
    printf("Udt client: Connect server successfully\n");
    return socket_fd;
}
void app_client_connect(int fd, int port){
    srand(time(NULL));
    Segment sendS,recvS;
    int c_isn = rand();
    char o_buffer[1020],i_buffer[1020];
    int client_port = myrand(1,65535);
    strcpy(sendS.l3info.SourceIpv4,"127.0.0.1");
    strcpy(sendS.l3info.DesIpv4,"127.0.0.1");
    sendS.l3info.protocol = 6;
    sendS.l4info.SeqNum = c_isn;
    sendS.l4info.AckNum = 0;
    sendS.l4info.Flag = 0x002;
    sendS.l4info.HeaderLen = 5;
    sendS.l4info.SourcePort = client_port;
    sendS.l4info.DesPort = port;
    sendS.l4info.WindowSize = 65535;
    _headermaker(&sendS);
    printsegment(&sendS);
    memcpy(o_buffer,sendS.header,20); 
    send(fd,o_buffer,sizeof(o_buffer),0);
    memset(o_buffer,0,sizeof(o_buffer));
    recv(fd,i_buffer,sizeof(i_buffer),0);
    parse_packet(i_buffer,&recvS);
    
    int s_isn = recvS.l4info.SeqNum;
    
    memcpy(&sendS,&recvS,sizeof(recvS));
    sendS.l4info.SeqNum = c_isn + 1;
    sendS.l4info.AckNum = s_isn + 1;
    sendS.l4info.Flag = 0x010;
    sendS.l4info.SourcePort = client_port;
    sendS.l4info.DesPort = 45525;
    _headermaker(&sendS);
    

    memcpy(o_buffer,sendS.header,20); 
    send(fd,o_buffer,sizeof(o_buffer),0);
    memset(o_buffer,0,sizeof(o_buffer));
    
    ///////receive data////////
    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }
    int packet_num = 0;
    int base = 0;
    while (1) {
        int bytesRead = recv(fd, i_buffer, sizeof(i_buffer), 0);
        if (bytesRead <= 0) {
            break; 
        }
        
        if(!corrupt(0)){
            parse_packet(i_buffer,&recvS);
            sendS.l4info.AckNum += (bytesRead-20)+1;
            sendS.l4info.Flag = 0x10;
            _headermaker(&sendS);
            printf("send ack %d: seg %d, ack %d\n",(sendS.l4info.AckNum-s_isn-1)/1000,sendS.l4info.SeqNum,sendS.l4info.AckNum);
            memcpy(o_buffer,sendS.header,20);
            send(fd,o_buffer,20,0);
            fwrite(i_buffer+20, 1, bytesRead-20, file);
            sendS.l4info.AckNum --;
        }
        else{
            printf("packet corrupt!,Resent packet!\n");
            printf("send ack %d: seg %d, ack %d\n",(sendS.l4info.AckNum-s_isn-1)/1000,sendS.l4info.SeqNum,sendS.l4info.AckNum);
            send(fd,o_buffer,20,0);
        }
    }
    printf("close\n");
    fclose(file);

}

int main(){
    int fd = udt_client_connect();
    app_client_connect(fd,45525);  
    while (1);
    
    
}