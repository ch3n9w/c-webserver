#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 2000000

int creatSocket(int);

void take_log(struct sockaddr_in * remote_addr);

int creatDaemon();

int worker(int);


