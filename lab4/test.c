#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "header.h"
int main(){
    srand(time(NULL));
    Segment sendS,recvS;
    uint32_t c_isn = rand();
    char o_buffer[1020],i_buffer[1020];
    int client_port = myrand(1,65535);
    strcpy(sendS.l3info.SourceIpv4,"127.0.0.1");
    strcpy(sendS.l3info.DesIpv4,"127.0.0.1");
    sendS.l3info.protocol = 6;
    sendS.l4info.SeqNum = 0;
    sendS.l4info.AckNum = 0;
    sendS.l4info.Flag = 0x002;
    sendS.l4info.HeaderLen = 5;
    sendS.l4info.SourcePort = client_port;
    sendS.l4info.DesPort = 45525;
    sendS.l4info.WindowSize = 65535;
    _headermaker(&sendS);
    printheader(recvS.header);

    parse_packet(sendS.header,&recvS);
    printheader(recvS.header);
    printsegment(&recvS);




    
}