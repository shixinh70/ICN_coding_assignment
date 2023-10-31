#include"header.h"

uint32_t myrand(int min,int max);
void randip(char* ip);

void mysend(int fd,char *output);
void itos(int num,char* str);

void headermaker(Segment* s);
void checksummaker(Segment* s);
void psuedoheadmaker(Segment* s);
void tcpheadermaker(Segment* s);
void createtestfile(Segment* test);
void headercompare(char* ans ,char* input, char* result);
void printheader(char* header);
void printsegment(Segment* s);


void checksummaker(Segment* s){

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
void psuedoheadmaker(Segment* s){
    uint32_t temp[3] = {0};
    temp[0] = inet_addr(s->l3info.SourceIpv4);
    temp[1] = inet_addr(s->l3info.DesIpv4);
    temp[2] = htonl((s->l4info.HeaderLen*4)+(s->l3info.protocol<<16) + 0);
    for(int i=0;i<3;i++){//htonl 把高記憶體位置的東西和低記憶體位置的東西互換 (只要照原本的高低位填，在互換就會得到要的header)
        memcpy(s->pseudoheader+4*i,temp+i,4);
    }
}
void tcpheadermaker(Segment* s){
    
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
void headermaker(Segment* s){
    
    tcpheadermaker(s);
    psuedoheadmaker(s);
    checksummaker(s);
    

}
void printheader(char* header){
    for(int i=0;i<20;i++){
        printf("%01X ",(uint8_t)header[i]);
    }
}
void printsegment(Segment* s){
    printf("Receive information:\n\n\
            Layer 3 information:\n\
            Source IP: %s , Destination Ip: %s \n\
            Protocol: %u (TCP)\n\n\
            Layer 4 information:\n\
            Source port: %u , Destination port: %u \n\
            Seq number: %u , Ack number: %u \n\
            Header length: %u (bytes) , FLAG: 0x%X (ACK) \n\
            Window size: %u \n\n",s->l3info.SourceIpv4,s->l3info.DesIpv4,s->l3info.protocol,\
            s->l4info.SourcePort,s->l4info.DesPort,s->l4info.SeqNum,\
            s->l4info.AckNum,s->l4info.HeaderLen,s->l4info.Flag,s->l4info.WindowSize);
}
void headercompare(char* ans ,char* input, char* result){
    int count = 0;
    int t2score = 0, t3score = 0;
    int sp=0,dp=0,seq=0,ack=0,len_flag=0,winsize=0,checksum = 0;   
    memset(result,0,strlen(result));
    int tag[20] = {0} ;
    sprintf(result,"\nThe ans header is:\n");
    for(int i=0;i<20;i++){

        if(ans[i]==input[i]){
            tag[i] = 1;
            count++;
        }
        sprintf(result+strlen(result),"%02X ",(uint8_t)ans[i]);
    }
    if(tag[0]&&tag[1]) {sp=1;t2score+=3;}
    if(tag[2]&&tag[3]) {dp=1;t2score+=3;}
    if(tag[4]&&tag[5]&&tag[6]&&tag[7]) {seq=1;t2score+=3;}
    if(tag[8]&&tag[9]&&tag[10]&&tag[11]) {ack=1;t2score+=3;}
    if(tag[12]&&tag[13]) {len_flag= 1;t2score+=3;}
    if(tag[14]&&tag[15]) {winsize=1;t2score+=3;}
    if(tag[16]&&tag[17]) {checksum=1;t3score+=12;}
    
    sprintf(result+strlen(result),"\n\nYour header is:\n");
    for(int i=0;i<20;i++){
        sprintf(result+strlen(result),"%2X ",(uint8_t)input[i]);
    }
    sprintf(result+strlen(result),"\n\nYou got %d/20 bytes correct!!\nTask1 gets 70pts, \
Task2 gets %dpts, Task3 gets %d pts\nTotal:%d",count,t2score,t3score,70+t2score+t3score);
    

}
uint32_t myrand(int min,int max){
    return(rand()%(max-min+1)+min);
}
void mysend(int fd,char *output){
    char buffer[1024]={0};
    strcpy(buffer,output);
    send(fd,buffer,strlen(buffer),0);
}
void itos(int num,char* str){
    int i, rem, len = 0, n;
    n = num;
    while (n != 0){
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++){
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}
void randip(char* ip){
    char temp[4];
    for(int i=0;i<4;i++){
        itos(myrand(1,255),temp);
        strcpy(ip+strlen(ip),temp);
        if(i!=3){
            strcpy(ip+strlen(ip),".");

        }
    }
}
void createtestfile(Segment* test){
    memset(test,0,sizeof(Segment));
    randip(test->l3info.SourceIpv4);
    randip(test->l3info.DesIpv4);
    test->l3info.protocol = 6;
    test->l4info.AckNum = rand()+1;
    test->l4info.SeqNum = rand()+1;
    test->l4info.DesPort= myrand(1,__UINT16_MAX__);
    test->l4info.SourcePort= myrand(1,__UINT16_MAX__);
    test->l4info.Flag = 0X10;
    test->l4info.HeaderLen = 5;
    test->l4info.WindowSize = myrand(1,__UINT16_MAX__);
}
void serverfunction(int clientfd){
    char ibuffer[1024] = {0};
    char obuffer[1024] = {0};
    mysend(clientfd,"Enter \"test\" to start!. or \"quit\" to leave!" );
        while(1){
            if(recv(clientfd,ibuffer,sizeof(ibuffer),0)<=0) break;
                if(strcmp(ibuffer,"test")==0){
                memset(ibuffer,0,sizeof(ibuffer));
                Segment s_test;
                createtestfile(&s_test);
                memcpy(obuffer,&s_test,sizeof(Segment));
                send(clientfd,obuffer,sizeof(Segment),0);//send test file
                memset(obuffer,0,sizeof(obuffer));

                char input_header[20];
                if(recv(clientfd,ibuffer,sizeof(ibuffer),0)<=0) break;//receive client's header
                memcpy(input_header,ibuffer,sizeof(input_header));
                memset(obuffer,0,sizeof(obuffer));
                headermaker(&s_test);
                headercompare(s_test.header,input_header,obuffer);

                send(clientfd,obuffer,strlen(obuffer),0);
                memset(obuffer,0,sizeof(obuffer));
                break;

                }else if (strcmp(ibuffer,"quit")==0){
                    break;

                }else {
                    mysend(clientfd,"Wrong input!!,enter \"test\" to start!." );
                }
            

        }
}

void receivedata(int sockfd,Segment* s){
    char obuffer[1024] = {0};
    char ibuffer[1024] = {0};
    char c = 0;
    while(recv(sockfd,ibuffer,sizeof(ibuffer),0)){
        printf("server: %s\n",ibuffer);
        scanf("%s",obuffer);
        send(sockfd,obuffer,strlen(obuffer),0);
       
        if(strcmp(obuffer,"test")==0) {
            memset(obuffer,0,sizeof(obuffer));
            printf("Receive Data from server!...\n");
            recv(sockfd,ibuffer,sizeof(ibuffer),0);
            memcpy(s,ibuffer,sizeof(Segment));
            printsegment(s);
            break;   
        }
        else if(strcmp(obuffer,"quit")==0){
            printf("close Socket\n");
            close(sockfd);
            exit(0);
        }
        
    }
}

void sendheader(int sockfd, char* header){
    char obuffer[1024] = {0};
    char ibuffer[1024] = {0};
    memcpy(obuffer,header,20);
    send(sockfd,obuffer,20,0);
    memset(obuffer,0,sizeof(obuffer));
    recv(sockfd,ibuffer,sizeof(ibuffer),0);
    printf("server: %s\n",ibuffer); 
}