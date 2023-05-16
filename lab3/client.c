#include "header.h"
typedef struct udt{
    int fd;
    int (*connector)();
}Udt;
int connector(){
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
    return socket_fd;
}
void udt_send(){
}
void udt_constructor(Udt* this){
    this->connector = connector;
    this->fd = this->connector();
}
int main(int argc , char *argv[]){
    Udt udt;
    udt_constructor(&udt);
    char i_buffer[1024];
    char o_buffer[1024];
    int packet_index = 0;
    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }
    
        while (1) {
        // 从客户端接收数据
        int bytesRead = recv(udt.fd, i_buffer, sizeof(i_buffer), 0);
        sprintf(o_buffer,"ack%d",++packet_index);
        printf("%s\n",o_buffer);
        send(udt.fd,o_buffer,strlen(o_buffer),0);
        if (bytesRead <= 0) {
            break; // 连接已关闭或出错
        }
        memset(o_buffer,0,sizeof(o_buffer));
        // 将接收到的数据写入文件
        fwrite(i_buffer, 1, bytesRead, file);
    }
    printf("close\n");
    fclose(file);
    close(udt.fd);
}