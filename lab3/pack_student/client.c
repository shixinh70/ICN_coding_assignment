#include "header.h"

int main(){
    /*---------------------------UDT SERVER----------------------------------*/
    srand(getpid());
        //Create socket.
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Create socket fail!\n");
        return -1;
    }

    //Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(45525)
    };
    int server_len = sizeof(serverAddr);

    //Connect to server's socket.
    if (connect(socket_fd, (struct sockaddr *)&serverAddr, server_len) == -1) {
        printf("Connect server failed!\n");
        close(socket_fd);
        exit(0);
    }
    /*---------------------------UDT SERVER-----------------------------------*/
    
    char header_s[20] ;
    char header_r[20] ;
    uint32_t seg = ntohl(rand());
    uint32_t ack = ntohl(0);
    uint16_t d_port = ntohs(45525);
    uint16_t s_port = ntohs(1234);
    memcpy(header_s+2,&d_port,2);
    memcpy(header_s,&s_port,2);
    memcpy(header_s+4,&seg,4);
    memcpy(header_s+8,&ack,4);
    header_s[12] = 5<<4;
    header_s[13] = SYN;
    printheader(header_s);
    send(socket_fd,header_s,20,0);
    recv(socket_fd,header_r,20,0);
    memcpy(&seg,header_r+8,4);
    memcpy(header_s+4,&seg,4);
    memcpy(&ack,header_r+4,4);
    ack+=(1<<24);
    memcpy(header_s+8,&ack,4);
    int flag = 0x10;
    memcpy(header_s+13,&flag,1);
    send(socket_fd,header_s,20,0);
    while(1);

    /*---------------------------3 way hand shake-----------------------------*/
    /*                                                                        */                                              
    /* TODO: Make a three way handshake with RDT server by using TCP header   */
    /*       char header[20] (lab2).                                          */
    /*       Make sure the SrcPort(Random), DesPort(45525), Seg#, Ack#, FLAG  */
    /*       are correct.                                                     */
    /*                                                                        */                                              
    /*---------------------------3 way hand shake-----------------------------*/
    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }
    
    /*----------------------------receive data--------------------------------*/
    /*                                                                        */                                              
    /* TODO: Receive data from the RDT server.                                */
    /*       Each packet will be 20bytes TCP header + 1000bytes paylaod       */
    /*       exclude the last one. (the payload may not be exactly 1000bytes) */
    /*                                                                        */
    /* TODO: Once you receive the packets, you should check whether it's      */                                                            
    /*       corrupt or not (checksum) , and send the corresponding ack       */                                                  
    /*       packet (also a char[20] ) back to the server.                    */
    /*                                                                        */
    /* TODO: fwrite the payload into a .jpg file, and check the picture.      */
    /*                                                                        */                                              
    /*----------------------------receive data--------------------------------*/



    /*-------------------------Something important----------------------------*/
    /* NOTE: TO make lab3 simple                                              */
    /*                                                                        */                                              
    /*       1. The SrcIP and DesIP are both 127.0.0.1,                       */
    /*          Tcp header length will be 20byts, windowsize = 65535 bytes    */                                              
    /*       2. The Handshake packets won't be corrupt.                       */
    /*       3. The packet will only corrupt but not miss or be disordered.   */                                              
    /*       4. Only the packets come from server may corrupt.(don't have to  */
    /*          worry that the ack sent by client will corrupt.)              */
    /*       5. We offer mychecksum() for you to verify the checksum, and     */
    /*          don't forget to verify pseudoheader part.                     */
    /*       6. Once server finish transmit the file, it will close the       */
    /*          client socket.                                                */                                              
    /*       7. You can adjust server by                                      */                                              
    /*          ./server {timeout duration} {corrupt probability}             */                                              
    /*                                                                        */                                              
    /*-------------------------Something important----------------------------*/

}