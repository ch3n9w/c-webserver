#include <signal.h>
#include <time.h>
#include "http.h" 
#include "action.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>

int creatSocket(int port){
    int server_fd;
    int option = 1;
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
	perror("socket failed!!");
	return -1;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){
	perror("bind failed!!!!");
	return -1;
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if(listen(server_fd, 30)<0){
	perror("listen failed");
	return -1;
    }
    return server_fd;
}

int creatDaemon(){
    pid_t pid = fork();
    if(pid<0){
	fprintf(stderr, "create daemon failed");
	return -1;
    }
    if(pid>0){
	/* the return value belongs to the parent process  */
	return 0;
    }
    /* signal(SIGTTOU,SIG_IGN); */
    /* signal(SIGHUP,SIG_IGN); */
    /* signal(SIGTSTP,SIG_IGN); */
    /* signal(SIGTTIN,SIG_IGN); */
    umask(0);
    setsid();
    /* close(0); */
    /* close(1); */
    /* close(2); */
    return 0;
}

int worker(int socket_fd){
    static unsigned char buf[BUF_SIZE];
    struct sockaddr_in ac_sockaddr;
    socklen_t addr_size=sizeof(ac_sockaddr);
    int ac_sockfd;
    while(1){
	if((ac_sockfd=accept(socket_fd, (struct sockaddr *)&ac_sockaddr, &addr_size))<0){
	    perror("connection failed");
	}
	take_log(&ac_sockaddr);
	pid_t pid = fork();
	if(pid==0){
	    memset(buf, '\0', sizeof(buf));
	    fprintf(stderr, "Check out whether buf is empty\n");
	    fprintf(stderr, "%s\n", buf);
	    fprintf(stderr, "Start to receive data\n");
	    int len = recv(ac_sockfd, buf, BUF_SIZE, 0);
	    /* in this function , the buf 's content will turn to response content */
	    int ans = handleRequest(buf);
	    /* printf("%s\n",buf); */
	    if(ans<0){
		return -1;
	    }
	    send(ac_sockfd, buf, ans, 0);
        fprintf(stderr, "sending back the data\n");
	    close(ac_sockfd);
	    
	}else if (pid<0)
	    memset(buf, '\0', sizeof(buf));
	    close(ac_sockfd);
    }
    return 0;
}

void take_log(struct sockaddr_in *remote_addr)
{
    char *from = inet_ntoa(remote_addr->sin_addr);
    char str[64];
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime(&t);
    sprintf(str, "%d/%d/%d %d:%d:%d\n", lt->tm_year + 1900, lt->tm_mon + 1,
            lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
    char logstr[64];
    sprintf(logstr, "\t%s", str);
    strcat(from, logstr);
    FILE *log;
    log = fopen("log.txt", "r");
    if (log)
    {
        log = fopen("log.txt", "a+");
        if (EOF == fputs(from, log))
        {
            fprintf(stderr, "写入错误");
            perror(" ");
            exit(-1);
        }
        fclose(log);
    }
    else
    {
        log = fopen("log.txt", "w+");
        if (EOF == fputs("IP地址\t\t\t 请求时间\n", log))
        {
            fprintf(stderr, "写入错误");
            perror(" ");
            exit(-1);
        }
        if (EOF == fputs(from, log))
        {
            fprintf(stderr, "写入错误");
            perror(" ");
            exit(-1);
        }

        fclose(log);
    }
}



