#include <stdio.h>
#include <stdlib.h>
#include "http.h"
#include "action.h"

// 主程序
int main(int argc, char * argv[]){
    // 判断参数是否合法
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // 端口范围必须合法
    int port = atoi(argv[1]);
    printf("Server is started and listening on port: %d\n", port);
    if (!port || port > 65535 || port < 0)
    {
        fprintf(stderr, "Please enter a correct port number\n");
        exit(EXIT_FAILURE);
    }

    // 根据端口号来创建socket
    int socket_fd = creatSocket(port);
    // socket 创建失败就报错
    if(socket_fd < 0){
        fprintf(stderr, "create socket failed");
        return 1;
    }

    // 将web目录设置在html下面
    if(chdir(WORK_DIR)==-1){
        fprintf(stderr, "can not switch to work directory");
        return 1;
    }

    #ifndef DEBUG
	    if(!~create_daemon()){
		    //创建守护进程失败！
		    exit(2);
	    }
	    //成为守护进程......
    #endif
    // 让socket开始工作, 去执行获取, 解析和响应请求的一系列操作
    if(worker(socket_fd)<0)
        fprintf(stder  r, "web server failed");

    return 1;

}


