#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 2000000

int creatSocket(int);

int creatDaemon();

int worker(int);


