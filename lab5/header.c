#include "header.h"
/////////////////////////common///////////////////////////
void initS(Segment* sendSegment,uint32_t currentSeg, uint32_t currentAck){
    ///l3info
    strcpy(sendSegment.l3info.SourceIpv4,"127.0.0.1");
    strcpy(sendSegment.l3info.DesIpv4,"127.0.0.1");
    sendSegment.l3info.protocol = 6;
    ///l4info
    sendS.l4info.SourcePort = SERVER_PORT;
    sendS.l4info.DesPort = cport;
    sendS.l4info.SeqNum = currentSeg;
    sendS.l4info.AckNum = currentAck;
    sendS.l4info.SourcePort = currentAck;
    sendS.l4info.HeaderLen = 5;
    sendS.l4info.Flag = SYNACK;
    sendS.l4info.WindowSize = 65525;
}
void parse_packet(char* recvbuffer,Segment* recvSegment){

    strcpy(recvSegment->l3info.DesIpv4,"127.0.0.1");
    strcpy(recvSegment->l3info.SourceIpv4,"127.0.0.1");
    recvSegment->l3info.protocol = 6;
    char header[20];
    memcpy(header,recvbuffer,20);
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
/////////////////////////common///////////////////////////


/////////////////////////server///////////////////////////

/////////////////////////server///////////////////////////

//////////////////////////client//////////////////////////

//////////////////////////client/////////////////////////