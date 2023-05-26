#include"header.h"
#define SERVER_PORT 45525
#define SYN 0X2
#define ACK 0X10 
#define SYNACK 0X12



int client_fd;
char i_buffer[20];
char o_buffer[1020];
Segment recvS,sendS;
uint32_t currentSeg, currentAck;

ssize_t bytesRead = 2, bytesRecv = 0;
int j = -1,nextacked = 0, count=0, this_round = 0;
Segment Seg_buffer[10];


void* thread_function(void* arg) {
    
    while(1){
        recvpacket(client_fd,i_buffer,sizeof(i_buffer),&recvS,"server");           
        if((recvS.l4info.SeqNum) != currentAck){
            printf("Rdt server: wrong SeqNum, drop packet\n");
        }
    int match = 0;
    for(int i=nextacked;i<this_round;i++){
        if(recvS.l4info.AckNum == Seg_buffer[i].l4info.SeqNum+Seg_buffer[i].p_len){
            printf("Rdt server: receive Ack %d\n",j*10+i);
            nextacked = i+1;
            match = 1;
            //break;
        }
    }
    if(!match){
        printf("Rdt server: receive wrong Ack (waiting for %d)\n",j*10+nextacked);
        match = 0;
    }
    }
    
}

int main(int argc, char *argv[]){

    srand(getpid());
    int socket_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (socket_fd < 0){
        printf("Server: Fail to create a socket.");
    }

    double timeout = 0.1, PCOR = 0.3;
    pthread_t thread_id;

    if(argc>1){
        timeout = (strtod((char* )argv[1],NULL));
        if(timeout>=1) timeout = 1;
        else if(timeout<=0.1) timeout = 0.1;
    }

    if(argc>2){
        PCOR = (strtod((char* )argv[2],NULL));
        if(PCOR>=1) PCOR = 1;
        else if(PCOR<=0) PCOR = 0;
    }
    

    printf("Set timeout duration = %0.2f seconds, corrupt probability = %.2f \n",timeout);


    
    
    
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
    
    struct sockaddr_in clientAddr;
    int client_len = sizeof(clientAddr);

    //Accept the connect request.
    char ipAddr[INET_ADDRSTRLEN];

    while(1){
        printf("Udt Server: waiting for client connect\n");
        client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&client_len);


        memset(i_buffer,0,sizeof(i_buffer));
        memset(o_buffer,0,sizeof(i_buffer));
        memset(&recvS,0,sizeof(recvS));
        memset(&sendS,0,sizeof(sendS));
        memset(Seg_buffer,0,sizeof(Seg_buffer));
        bytesRead =2; bytesRecv =0; j =-1; nextacked =0; count=0; this_round =0;
        currentAck =0 ; currentSeg=0 ;
    
        
        inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddr, INET_ADDRSTRLEN);//Get client ipaddr(string);
        printf("Udt Server: accept client from %s:%d\n",ipAddr,ntohs(clientAddr.sin_port));
        /*---------------Rdt Server----------------*/
        
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
        if(packet_corrupt(&recvS,"server")){
            printf("Rdt server: drop packet\n");
            close(client_fd);
            continue;
        };
        if(recvS.l4info.DesPort!=SERVER_PORT||recvS.l4info.Flag!=ACK){
            if(recvS.l4info.DesPort!=SERVER_PORT) printf("Rdt server: Port %u is closed!\n",recvS.l4info.DesPort);
            else printf("Rdt server: Not a ACK packet(0x2)\n");
            printf("Rdt server: drop packet\n");
            close(client_fd);
            continue;
        }
        else{
            printf("Rdt server: connection established!, ready to transmit data\n");
            dPort = recvS.l4info.SourcePort;
            currentSeg = rand();
            currentAck = recvS.l4info.SeqNum+1;
            initS(&sendS,SERVER_PORT,dPort);
            replyS(&sendS,currentSeg,currentAck,SYNACK);
            //sendpacket(client_fd,o_buffer,sizeof(o_buffer),&sendS,"server",0);
        }
        /*-----------------------Transmit Data----------------------*/
        

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
            this_round = 0;
            nextacked = 0;
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
                this_round++;
                currentSeg += bytesRead;
            }
            
            while(nextacked<this_round){
                
                int result = pthread_create(&thread_id, NULL, thread_function, NULL);
                if (result != 0) {
                    printf("pthread_create failed\n");
                    return 1;
                }

                
                usleep(timeout*1000000);
               
                result = pthread_cancel(thread_id);
                if (result != 0) {
                    printf("pthread_cancel failed\n");
                    return 1;
                }
                pthread_join(thread_id, NULL);
                if(nextacked>=this_round) break;
                printf("\nRdt server: ------------Timepout! retransmit packet!------------\n");
                sendpacket(client_fd,o_buffer,(Seg_buffer+nextacked)->p_len+20,Seg_buffer+nextacked,"server",PCOR);
            }
        }
        printf("Rdt server: finish transmition...!\n\n");
        
        sleep(1);
        printf("Rdt server: close Rdt client\n");
        close(client_fd);
        /*-----------------------Transmit Data----------------------*/
        
    }

}