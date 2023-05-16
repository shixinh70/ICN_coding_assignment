#include "header.h"
int main(int argc , char *argv[]){  
    
    int socket_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (socket_fd < 0){
        printf("Fail to create a socket.");
    }

    // Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(45525)
    };

    //Bind socket to the address.
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(socket_fd);
        exit(0);
    }
    
    //Listening the socket.
    if (listen(socket_fd, 5) == -1) {
        printf("socket %d listen failed!\n", socket_fd);
        close(socket_fd);
        exit(0);
    }
    char o_buffer[1024];
    char i_buffer[1024];
    int client_fd;
    struct sockaddr_in clientAddr;
    int client_len = sizeof(clientAddr);

    
    //Accept the connect request.
    while(1){
        client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&client_len);
        FILE* file = fopen("image.jpg", "rb");
        if (file == NULL) {
            perror("fail to open");
            exit(1);
        }
        size_t bytesRead;
        while ((bytesRead = fread(o_buffer, 1, sizeof(o_buffer), file)) > 0) {
            if (send(client_fd, o_buffer, bytesRead, 0) < 0) {
                perror("Fail to send");        
                exit(1);
            }
            memset(i_buffer,0,sizeof(i_buffer));
            recv(client_fd,i_buffer,sizeof(i_buffer),0);
            printf("send %s\n",i_buffer);
            
        }
        fclose(file);
        close(client_fd);
    }
}