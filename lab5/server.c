#include"header.h"
#define SERVER_PORT 45525
#define SYN 0X2
#define ACK 0X10 
#define SYNACK 0X12
#define PCOR 0.3
int timeout = 0;
int get_packet = 0;
void timeout_handler(int sig)
{
    timeout = 1;
    printf("Timeout!\n");
    
}

void* thread_function(void* arg) {
    while(get_packet){};
    ThreadArgs* args = (ThreadArgs*)arg;
    //recvpacket(args->fd,args->buffer,args->buff_len,args->recvS,"server");
    get_packet = 1;
    
    
    return NULL;
}

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
        perror("Udt Server: Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    
    //Listening the socket.
    if (listen(socket_fd, 5) == -1) {
        printf("Udt Server: socket %d listen failed!\n", socket_fd);
        close(socket_fd);
        exit(0);
    }
    int client_fd;
    struct sockaddr_in clientAddr;
    int client_len = sizeof(clientAddr);

    //Accept the connect request.
    char ipAddr[INET_ADDRSTRLEN];

    while(1){
        printf("Udt Server: waiting for client connect\n");
        client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&client_len);
        inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddr, INET_ADDRSTRLEN);//Get client ipaddr(string);
        printf("Udt Server: accept client from %s:%d\n",ipAddr,ntohs(clientAddr.sin_port));
        /*---------------Rdt Server----------------*/
        char i_buffer[20];
        char o_buffer[1020];
        Segment recvS,sendS;
        uint32_t currentSeg, currentAck;
        uint16_t dPort;

        printf("Rdt server: Hello, This is Rdt server on 127.0.0.1:45525\n");
        printf("Rdt server: Waiting for the client to connect\n");
        /*--------------3handshake-----------------*/
        /*------------------SYN--------------------*/
        recvpacket(client_fd,i_buffer,sizeof(i_buffer),&recvS,"server");

        if(packet_corrupt(&recvS,"server")){
            printf("Rdt server: drop packet\n");
            close(client_fd);
            continue;
        };
        if(recvS.l4info.DesPort!=SERVER_PORT||recvS.l4info.Flag!=SYN){
            if(recvS.l4info.DesPort!=SERVER_PORT) printf("Rdt server: Port %u is closed!\n",recvS.l4info.DesPort);
            else printf("Rdt server: Not a SYN packet(0x2)\n");
            printf("Rdt server: drop packet\n");
            close(client_fd);
            continue;
        }
        else{
            printf("Rdt server: handshaking with client at 127.0.0.1:%u\n", recvS.l4info.SourcePort);
            dPort = recvS.l4info.SourcePort;
            currentSeg = rand();
            currentAck = recvS.l4info.SeqNum+1;
            initS(&sendS,SERVER_PORT,dPort);
            replyS(&sendS,currentSeg,currentAck,SYNACK);
            sendpacket(client_fd,o_buffer,sizeof(o_buffer),&sendS,"server",0);
        } 
        recvpacket(client_fd,i_buffer,sizeof(i_buffer),&recvS,"server");
        /*-----------------------Transmit Data----------------------*/
        ssize_t bytesRead = 2, bytesRecv = 0;
        int j = -1,lastacked = -1, count=0, round = 0;
        Segment Seg_buffer[10];
        pthread_t thread;
        int result;

        FILE* file = fopen("image.jpg", "rb");
        if (file == NULL) {
            perror("fail to open");
            exit(1);
        }
        currentSeg = recvS.l4info.AckNum;
        currentAck = recvS.l4info.SeqNum;

        
        while(bytesRead>1){
            printf("\nRdt server: ------------pipeline send packet!------------\n");
            j++;
            round = 0;
            for(int i=0;i<10;i++){
                bytesRead = fread(sendS.payload, 1, 1000, file);
                if(bytesRead<1) break;
                sendS.l4info.SeqNum = currentSeg;
                sendS.l4info.AckNum = currentAck;
                sendS.l4info.Flag = ACKPSH;
                sendS.p_len = bytesRead;
                memcpy(Seg_buffer+i,&sendS,sizeof(Segment));//buffer packet
                sendpacket(client_fd,o_buffer,sendS.p_len+20,&sendS,"server",PCOR);
                count++;
                round++;
                currentSeg += bytesRead;
            }
            lastacked = -1;
                
                printf("\nRdt server: ------------Start receive Ack packet!--------------\n");
                for(int i =0;i<round;i++){
                    recvpacket(client_fd,i_buffer,sizeof(i_buffer),&recvS,"server");
                    
                    if((recvS.l4info.SeqNum) != currentAck){
                        printf("Rdt server: wrong SeqNum, drop packet\n");
                    }
                    int match = 0;
                    for(int i=0;i<round;i++){
                        if(recvS.l4info.AckNum == Seg_buffer[i].l4info.SeqNum+Seg_buffer[i].p_len){
                            printf("Rdt server: receive Ack %d\n",j*10+i);
                            lastacked = i;
                            match = 1;
                            break;
                        }
                    }
                    if(!match){
                        printf("Rdt server: receive wrong Ack (waiting for %d)\n",j*10+lastacked+1);
                        match = 0;
                    }
                
                }
            
            printf("\nRdt server: ------------Timepout! retransmit packet!------------\n");
            while(lastacked+1<round){
            
                sendpacket(client_fd,o_buffer,(Seg_buffer+lastacked+1)->p_len+20,Seg_buffer+lastacked+1,"server",PCOR);
                recvpacket(client_fd,i_buffer,sizeof(i_buffer),&recvS,"server");
                
                if((recvS.l4info.SeqNum) != currentAck){
                    printf("Rdt server: wrong SeqNum, drop packet\n");
                }

                if(recvS.l4info.AckNum == Seg_buffer[lastacked+1].l4info.SeqNum + Seg_buffer[lastacked+1].p_len){
                    printf("Rdt server: receive Ack %d\n",j*10+lastacked+1);
                    lastacked ++;
                    
                }
                else printf("Rdt server: receive wrong Ack (waiting for %d)\n",j*10+lastacked+1);
                sleep(1);
            }
        }
        close(client_fd);
        /*-----------------------Transmit Data----------------------*/

        printf("Rdt server: close Rdt client\n");
    }

}