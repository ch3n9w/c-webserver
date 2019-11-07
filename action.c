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


// 创建socket函数
int creatSocket(int port){
    int server_fd;
    int option = 1;
    // 创建失败就报错
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed!!");
        return -1;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);

    // 绑定失败也报错
    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){
        perror("bind failed!!!!");
        return -1;
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    // socket 开始监听
    if(listen(server_fd, 30)<0){
        perror("listen failed");
        return -1;
    }
    // 返回socket文件标识符
    return server_fd;
}

int create_daemon(){//创建守护进程
	pid_t pid=fork();
	
	if(pid<0){//fork 失败
		fprintf(stderr,"create daemon process fail!! error num:%d \"%s\"\n",errno,strerror(errno));
		return -1;
	}
	if(pid>0){//父进程运行到这里就停止
		fprintf(stderr,"the main pid is: %d\n",pid);
		exit(0);
	}
	//fprintf(stderr,"create daemon successful! the son pid= %d\n\n",getpid());

	signal(SIGTTOU,SIG_IGN);
	signal(SIGHUP,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	umask(0); //将文件掩码设置为0
	setsid(); //子进程创建一个新的会话, 并且脱离原先的终端
	//chdir("/");
	close(0);//关闭所有打开的文件描述符
	close(1);
	close(2);
	return 0;
}

// 工作函数
int worker(int socket_fd){
    static unsigned char buf[BUF_SIZE];
    struct sockaddr_in ac_sockaddr;
    socklen_t addr_size=sizeof(ac_sockaddr);
    int ac_sockfd;
    while(1){
        // 接受socket请求
        if((ac_sockfd=accept(socket_fd, (struct sockaddr *)&ac_sockaddr, &addr_size))<0){
            perror("connection failed");
        }
        // 将这次请求的情况写入日志中
        take_log(&ac_sockaddr);
        // fork进程
        pid_t pid = fork();
    
        if(pid==0){
            // 如果是子进程的话
            memset(buf, '\0', sizeof(buf));

            int len = recv(ac_sockfd, buf, BUF_SIZE, 0);
            // 处理请求
            int ans = handleRequest(buf);
            if(ans<0){
                return -1;
            }
            // 将响应内容返回给客户端
            send(ac_sockfd, buf, ans, 0);
            close(ac_sockfd);
        
        }else if (pid<0)
            memset(buf, '\0', sizeof(buf));
        // 处理完之后关闭socket连接
        close(ac_sockfd);
    }
    return 0;
}

// 写日志的函数
void take_log(struct sockaddr_in *remote_addr){
    char *from = inet_ntoa(remote_addr->sin_addr);
    char str[64];
    time_t t;
    struct tm *lt;
    time(&t);
    lt = localtime(&t);
    // 记录的是客户端的ip和请求的时间
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
	fputs(from, log);
        if (EOF == fputs(from, log))
        {
            fprintf(stderr, "写入错误");
            perror(" ");
            exit(-1);
        }
        fclose(log);
	fprintf(stderr, "%s", from);
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



