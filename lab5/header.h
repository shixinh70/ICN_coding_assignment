/////////////////////////shared//////////////
#pragma once
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
typedef struct l4info{
    uint32_t SourcePort,DesPort,SeqNum,AckNum,HeaderLen,Flag,WindowSize,CheckSum;
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
typedef struct connectInfo{
    uint16_t sPort,cPort;
}ConnectInfo;

void parse_packet(char* recvbuffer,Segment* recvSegment);
void initS(Segment* sendSegment,uint32_t currentSeg, uint32_t currentAck);
/////////////////////////shared///////////////////////

/////////////////////////server/////////////////////////


/////////////////////////server///////////////////////////

//////////////////////////client//////////////////////////

//////////////////////////client/////////////////////////