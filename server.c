#include <stdio.h>
#include <stdlib.h>
#include "http.h"
#include "action.h"


int main(int argc, char * argv[]){
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    printf("Server is started and listening on port: %d\n", port);
    if (!port || port > 65535 || port < 0)
    {
        fprintf(stderr, "Please enter a correct port number\n");
        exit(EXIT_FAILURE);
    }

    int socket_fd = creatSocket(port);
    if(socket_fd < 0){
	fprintf(stderr, "create socket failed");
	return 1;
    }

/* #ifndef DEBUG */
    /* if(creatDaemon()<0){ */
	/* fprintf(stderr, "create daemon failed"); */
    /* } */
/* #endif */
    if(chdir(WORK_DIR)==-1){
	fprintf(stderr, "can not switch to work directory");
	return 1;
    }

    if(worker(socket_fd)<0)
	fprintf(stderr, "web server failed");

    return 1;

}


