#include "header.h"
/////////////////////////common///////////////////////////




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
void initS(Segment* sendSegment,uint16_t sPort,uint16_t dPort){
    ///l3info
    sendSegment->p_len = 0;
    strcpy(sendSegment->l3info.SourceIpv4,"127.0.0.1");
    strcpy(sendSegment->l3info.DesIpv4,"127.0.0.1");
    sendSegment->l3info.protocol = 6;
    ///l4info
    sendSegment->l4info.SourcePort = sPort;
    sendSegment->l4info.DesPort = dPort;
    sendSegment->l4info.HeaderLen = 5; 
    sendSegment->l4info.WindowSize = 65525;
}
void parse_packet(char* recvbuffer,Segment* recvSegment){
    strcpy(recvSegment->l3info.DesIpv4,"127.0.0.1");
    strcpy(recvSegment->l3info.SourceIpv4,"127.0.0.1");
    recvSegment->l3info.protocol = 6;
    char header[20];
    memcpy(header,recvbuffer,20);
    memcpy(recvSegment->header,recvbuffer,20);
    memcpy(recvSegment->payload,recvbuffer+20,recvSegment->p_len);
    uint32_t header_32[20] = {0};
    for(int i=0;i<20;i++){
        header_32[i] = (uint8_t)header[i];
    }
    recvSegment->l4info.SourcePort = (header_32[0]<<8)+header_32[1];
    recvSegment->l4info.DesPort = (header_32[2]<<8)+header_32[3];
    recvSegment->l4info.SeqNum = (header_32[4]<<24)+(header_32[5]<<16)+(header_32[6]<<8)+header_32[7];
    recvSegment->l4info.AckNum = (header_32[8]<<24)+(header_32[9]<<16)+(header_32[10]<<8)+header_32[11];
    recvSegment->l4info.HeaderLen = (header_32[12]>>4);
    recvSegment->l4info.Flag =header_32[13];
    recvSegment->l4info.WindowSize = (header_32[14]<<8)+header_32[15];
    recvSegment->l4info.CheckSum = (header_32[16]<<8)+header_32[17];
}



void replyS(Segment* sendSegment,uint32_t seg,uint32_t ack, uint16_t flag){
    sendSegment->l4info.SeqNum = seg;
    sendSegment->l4info.AckNum = ack;
    sendSegment->l4info.Flag = flag;
}
void printheader(char* header){
    for(int i=0;i<20;i++){
        printf("%X ",(uint8_t)header[i]);
    }
}
void sendpacket(int fd,char* buffer,int buf_len,Segment* sendS,char* tag){
    _headermaker(sendS);
    memcpy(buffer,sendS->header,sizeof(sendS->header));
    if(sendS->p_len!=0){
        memcpy(buffer+sizeof(sendS->header),sendS->payload,sendS->p_len);
    }
    printf("Rdt %s: send packet seg= %u, ack= %u, len= %u to port:%u\n",tag,sendS->l4info.SeqNum,sendS->l4info.AckNum,sendS->p_len,sendS->l4info.DesPort);
    send(fd,buffer,buf_len,0);

};
ssize_t recvpacket(int fd,char* buffer,int buff_len,Segment* recvS,char* tag){
    int byterecv = recv(fd,buffer,buff_len,0);
    if(byterecv==0){
        printf("Rdt client: Server close socket\n");
        close(fd);
        exit(0);
    }
    int p_len = byterecv-20;
    recvS->p_len = p_len;
    parse_packet(buffer, recvS);
    printf("Rdt %s: receive packet seg= %u, ack= %u, len= %u from port:%u\n",tag,recvS->l4info.SeqNum,recvS->l4info.AckNum,p_len,recvS->l4info.DesPort);
    return byterecv;
}
int packet_corrupt(Segment s,char* tag){
    uint16_t packet_checksum,compute_checksum;
    memcpy(&packet_checksum,s.header+16,2);
    _headermaker(&s);
    memcpy(&compute_checksum,s.header+16,2);
    packet_checksum = ntohs(packet_checksum);
    compute_checksum = ntohs(compute_checksum);
    if(packet_checksum!=compute_checksum){
        printf("%x %x\n",packet_checksum,compute_checksum);
        printf("Udt %s: Checksum not fit, packet corrupt\n",tag);
        return 1;
    }
    else return 0;
    
    
}
////////////////////////common///////////////////////////


/////////////////////////server///////////////////////////

/////////////////////////server///////////////////////////

//////////////////////////client//////////////////////////

//////////////////////////client/////////////////////////