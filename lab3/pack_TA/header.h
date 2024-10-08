/////////////////////////shared//////////////
#pragma once
#include <sys/socket.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#define SERVER_PORT 45525
#define SYN 0X2
#define ACK 0X10 
#define SYNACK 0X12 
#define ACKPSH 0X18

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
    char payload[1000];
    int p_len;
    L3info l3info;
    L4info l4info;
}Segment;
uint16_t mychecksum(char* buffer, int buffer_len);

int corrupt(double probability);
void replyS(Segment* sendSegment,uint32_t seg,uint32_t ack, uint16_t flag);
void parse_packet(char* recvbuffer,Segment* recvSegment);
void initS(Segment* sendSegment,uint16_t sPort,uint16_t dPort);
uint16_t _checksummaker1(Segment* s);
void _psuedoheadmaker(Segment* s);
void _tcpheadermaker(Segment* s);
void _headermaker(Segment* s);
void sendpacket(int fd,char* buffer,int buf_len,Segment* sendS,char* tag,double prob);
ssize_t recvpacket(int fd,char* buffer,int buff_len,Segment* recvS,char* tag);
int packet_corrupt(char* i_buffer,int i_buffer_len,char* tag);
void printheader(char* header);
uint16_t _checksummaker2(Segment* s);