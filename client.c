#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#define PORT 12335

int main(){
    int client_fd;
    char *hello = "hello world";
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if((client_fd=socket(AF_INET, SOCK_STREAM, 0))<0){
	perror("socket create failed");
	return -1;
    }
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    
    if(inet_pton(AF_INET, "127.0.0.1",&serv_addr.sin_addr)<=0){
	fprintf(stderr, "invalid address\n");
	return -1;
    }
    if(connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
	fprintf(stderr, "connection failed");
	return -1;
    }
    send(client_fd, hello, strlen(hello), 0);
    int valread = read(client_fd, buffer ,1024);
    fprintf(stderr, buffer);
}
