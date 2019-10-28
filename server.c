#include <stdio.h>
#include "http.h"
#include "action.h"


int main(int argc, char * argv[]){
    int socket_fd = creatSocket(12334);
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


