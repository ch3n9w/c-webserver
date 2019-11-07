#include <string.h>
#include <malloc.h>
#include "http.h"
#include "action.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

const char * httpVersion="HTTP/1.1";
const char * serverName="Kapache";

struct fileList * listFiles(const char *path);
struct fileList * node;

int len = 0;
int cgiFlag= 0;
static httpRequest request;
static httpResponse response;

static char content[BUF2_SIZE];
static unsigned char url_parse[20];
char cgiOut[BUF2_SIZE];

// 处理请求主程序
int handleRequest(unsigned char* buf){

    int folderFlag = 0;
    FILE * fp;

	// 如果解析异常就返回500 响应
    if(parseRequest(buf) == -1){
	response.status = 500;
	fp = fopen("500.html", "rb");

    }else if(parseRequest(buf) == 1){
		// 如果解析的结果显示请求的资源是一个目录的话就打开目录
		// fprintf(stderr, "this is the request folder: %s\n", request.url);
		node = listFiles(request.url);

		folderFlag = 1;
		fp = fopen("template.html", "r");
    }else if((fp = fopen(request.url, "rb"))==NULL){
		// 如果请求的是文件但是这个文件不存在, 就返回404
		response.status = 404;
		fp = fopen("404.html","rb");
	#ifdef DEBUG
		fprintf(stderr, "404 error occured");
	#endif
		// 将返回的Content-Type重置为text/html
		memset(response.contentType, '\0', sizeof(response.contentType));
		strcpy(response.contentType, "text/html");
    }else{
		// 打开文件成功, 设置返回状态码为200
		response.status = 200;
		// 如果打开的文件是cgi文件就设置标志位来供后续处理
		if(parseRequest(buf) == 2)
			cgiFlag = 1;
	#ifdef DEBUG
		fprintf(stderr, "200 ok, get the file");
	#endif
    }

	// 文件读取完毕, 开始生成响应内容
    int len, headLen, len1, len2;
    char folderBuf[100000];
    if(fp){
		// 清空缓存内容
		memset(content, '\0', BUF2_SIZE);
		memset(folderBuf, '\0', 100000);
		memset(buf, '\0', BUF_SIZE);

		// 如果目标资源是目录的话, 就将其中的每一项都用html渲染成web超链接
		if(folderFlag == 1){
			strcpy(content, "<html><body><table><tr><a>Name</a></tr><tr><td valign='top'></td><td><a href='/'>Parent Directory/</a></td>");
			len = strlen(content);
			char * item = "<tr><td valign='top'></td><td><a href='/%s'>%s</a></td>";
			char item_tmp[100];
			char * ender = "</table></body></html>";

			for(struct fileList* p=node;p->p_next;p=p->p_next){
				fprintf(stderr, "%s\n", p->name);
			}

			// 全部格式化渲染
			do{
				// fprintf(stderr, "start to print a node");
				// fprintf(stderr, "%s\n", node->name);
				sprintf(item_tmp, item, node->name, node->name);
				node = node->p_next;
				strcpy(content + len, item_tmp);
				len = strlen(content);
			}while(node->p_next);
			strcpy(content + len, ender);
			len = strlen(content);
			/* sprintf(folderBuf, content, "index.html"); */
			// fprintf(stderr, "%s", content);

			// 根据内容长度获取响应头
			headLen = get_head(buf, len);
		}else if(cgiFlag == 0){
			// 如果获取的是普通的静态文件, 读取
			len =fread(content, sizeof(unsigned char), BUF2_SIZE, fp);
			headLen = get_head(buf, len);
		}else{
			// 如果是cgi文件, 执行
			while(!feof(fp)) {
				if(fgets(cgiOut, 128, fp)!=NULL)
					printf("%s", cgiOut);
			}
			FILE * ptr;
			char buf_ps[1024];
			if((ptr=popen(cgiOut, "r"))!=NULL){
				rewind(ptr);
				while(!feof(ptr)){
					fgets(buf_ps, 1024, ptr);
					strcat(content, buf_ps);
				}
				pclose(ptr);
				ptr = NULL;
			}
			len = strlen(content);
			headLen = get_head(buf, len);
		}
	}else{
		// 默认打开index.html页面
		fp = fopen("index.html", "rb");
		len = fread(content, sizeof(unsigned char), BUF2_SIZE, fp);
		headLen = get_head(buf, len);
	}

	// 将content拼接到返回的响应头后面
	int i=0;
	for(i=0; i<len; i++){
	buf[headLen + i] = content[i];
	if((i+headLen) == BUF_SIZE){
		// 超出长度限制就报错
		fprintf(stderr, "the content length is beyond the limitation");
	}
	}
	// fprintf(stderr, "%d\n", len);
	// fprintf(stderr, "%d\n", headLen);
	// fprintf(stderr, "%s\n", buf);

	return len+headLen;

}

// 解析http请求
int parseRequest(unsigned char* buf){
    memset(request.url, '\0', URL_MAX_LEN);
    *(request.url) = '.';
	// 只接受GET请求, 其他类型的拒绝
    if(!(buf[0]=='G' && buf[1]=='E' && buf[2]=='T')){
		fprintf(stderr, "can not handle request other than GET method , return 500\n");
		return -1;
    }else if (!(buf[4]=='/')){
		fprintf(stderr, "url parse error , the path should start with /.\n");
		return -1;
    }else{
		int i;
		int pathLen=0;
		int flag=0;
		char ext[10];
		memset(ext, '\0', 10);
		char * extp=ext;
		char * bp = request.url +1;
		// 读http请求所指向的资源
		for(i=4; i< BUF2_SIZE; i++){
			if(buf[i]!=' '){
				*bp = buf[i];
				++bp;
				if(flag == 1){
					*extp = buf[i];
					++extp;
				}
			// 判断该文件是否有后缀名
			if(buf[i]=='.'){
				flag = 1;
			}

			}else{
				/* read file finished ! */
				if(flag==1){
					// fprintf(stderr, "the open file path is :%s \n", request.url);
					// fprintf(stderr, "the ext is :%s \n", ext);

					// 根据不同的后缀名来设置不同的响应头的Content-Type
					if(strcmp(ext,"css")==0)
						strcpy(response.contentType, "text/css");
					else if(strcmp(ext, "js")==0)
						strcpy(response.contentType, "application/javascript");
					else if(strcmp(ext, "jpg")==0)
						strcpy(response.contentType, "image/jpeg");
					else if(strcmp(ext, "ico")==0)
						strcpy(response.contentType, "image/ico");
					else if(strcmp(ext, "png")==0)
						strcpy(response.contentType, "image/png");
					else if(strcmp(ext, "cgi")==0){
						strcpy(response.contentType, "text/html");
						return 2;
					}
					else strcpy(response.contentType, "text/html");
				}else{
					strcpy(response.contentType, "text/html");
					// 返回值设置为1 表示该资源是文件夹类型
					return 1;
				}
				break;
			}
		}
#ifdef DEBUG
		fprintf(stderr, "\n%s\n",request.url);
		fprintf(stderr, "\n%s\n",ext);
#endif
		return 0;
    }

    return -1;
}

// 生成响应头
int get_head(unsigned char *buf, int contentLen){
    memset(buf, '\0', sizeof(buf));
    strcpy((char*)buf, httpVersion);
    int len=strlen((char*)buf);
    if(response.status==200)strcpy((char*)(buf+len), " 200 OK\r\n");
    else if(response.status==500)strcpy((char*)(buf+len), " 500 Internal Server Error\r\n");
    else if(response.status==404)strcpy((char*)(buf+len), " 404 Not Found\r\n");

    len += strlen((char*)(buf+len));
    sprintf((char*)(buf+len), "Server: %s\r\nContent-Length: %d\r\nContent-Type: %s; charset=UTF-8\r\n\r\n", serverName, contentLen, response.contentType);
    return len+strlen((char*)(buf+len));
}

// 将给定目录下的文件列出来存进一个链表中
struct fileList * listFiles(const char *basepath){
    struct dirent *dp;
    DIR *dir = opendir(basepath);

    struct fileList * node;
	// 声明链表的头节点
    node = (struct fileList *)malloc(sizeof(struct fileList));
    struct fileList * nodeHead = node;
    if(!dir)
	return node;
    while((dp = readdir(dir))!=NULL){
	if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name, "..") != 0){
	    node ->name = (char*)malloc(strlen(dp->d_name)+1);
	    strcpy(node->name,dp->d_name);
	    struct fileList * nodeNext;
	    nodeNext = (struct fileList *)malloc(sizeof(struct fileList));
	    node->p_next = nodeNext;
	    node = nodeNext;
	}
    }
    closedir(dir);

	// 返回链表的头指针
    return nodeHead;
}

