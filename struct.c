#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct info{
    // to do 
}Info;
void timer(int sec); //to do
ssize_t udt_send(int __fd, const void *__buf, size_t __n, int __flags);
ssize_t udt_recv();
void createpacket(char* packet, Info* info);
void app_sendFile(char* fileName, int pipeline, int mss){
    char* buffer = (char*)malloc((mss+20)*sizeof(char));
    FILE* file;// to do open file;
    size_t bytesRead;
    Info info;//to do 
    int fd;//to do
    while(!feof(file)){
        for(int i=0;i<pipeline;i++){
            bytesRead = fread(buffer, 1, sizeof(buffer), file);
            if(bytesRead <= 0) break;
            createpacket(buffer,&info);
            udt_send(fd,buffer,strlen(buffer),0);
            memset(buffer,0,sizeof(buffer));
            timer(-1);
        }
        udt_recv();
        
    }
    
    
}


int main(){
    udt_connect();
    App app;
    Info info;
    app.app_connect();
    app.app_send("file name",info);
    send()
}

size_t bytesRead;
        while ((bytesRead = fread(o_buffer, 1, sizeof(o_buffer), file)) > 0) {
            if (send(client_fd, o_buffer, bytesRead, 0) < 0) {
                perror("Fail to send");        
                exit(1);
            }