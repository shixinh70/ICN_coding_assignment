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
void printsegment(Segment* s);
void printheader(char* header);
void parse_packet(char* recvbuffer,Segment* recvSegment);
void _checksummaker(Segment* s);
void _psuedoheadmaker(Segment* s);
void _tcpheadermaker(Segment* s);
void _headermaker(Segment* s);
uint32_t myrand(int min,int max);