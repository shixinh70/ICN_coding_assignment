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
    uint32_t AckNum,SeqNum,SourcePort,DesPort,Flag,HeaderLen,WindowSize,Checksum;
}L4info;

typedef struct l3info{
    char SourceIpv4[16], DesIpv4[16];
    uint32_t protocol;
}L3info;

typedef struct Segment{
    char header[20];
    char _pseudoheader[12];
    L3info l3info;
    L4info l4info;
}Segment;

uint32_t myrand(int min,int max);
void _checksummaker(Segment* s,char* code);
void _psuedoheadmaker(Segment* s,char* code);
void _tcpheadermaker(Segment* s,char* code);
void _headermaker(Segment* s,char* code);
void printheader(char* header);
void printsegment(Segment* s);
void headercompare(char* ans ,char* input, char* result);
void mysend(int fd,char *output);
void createtestfile(Segment* test);
void serverfunction(int clientfd);
void receivedata(int sockfd,Segment* s);
void sendheader(int sockfd, char* header);
void itos(int num,char* str);
void randip(char* ip);