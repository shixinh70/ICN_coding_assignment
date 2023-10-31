#include "header.h"

int main(int argc , char *argv[]){
    srand(time(NULL)); 
    
    int socket_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (socket_fd < 0){
        printf("Fail to create a socket.");
    }
    int reuse = 1;
    //Avoid time wait. 
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
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
    char buffer[1024];
    int client_fd;
    struct sockaddr_in clientAddr;
    int client_len = sizeof(clientAddr);

    //Accept the connect request.
    while(1){
        client_fd = accept(socket_fd, (struct sockaddr *)&clientAddr, (socklen_t*)&client_len);
        
        printf("New connection\n");
        strcpy(buffer,"Hi, I'm server 111065540...");
        send(client_fd,buffer,strlen(buffer),0);
        memset(buffer,0,sizeof(buffer));
        sleep(1);

        serverfunction(client_fd);
        close(client_fd);
    }

}
