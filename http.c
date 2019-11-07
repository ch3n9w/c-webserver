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
/* char allFile[10000] = {'\0'}; */
int len = 0;
int cgiFlag= 0;
static httpRequest request;
static httpResponse response;

static char content[BUF2_SIZE];
static unsigned char url_parse[20];
char cgiOut[BUF2_SIZE];

int handleRequest(unsigned char* buf){

    int folderFlag = 0;
    FILE * fp;
    if(parseRequest(buf) == -1){
	response.status = 500;
	fp = fopen("500.html", "rb");

    }else if(parseRequest(buf) == 1){
	fprintf(stderr, "this is the request folder: %s\n", request.url);
	node = listFiles(request.url);

	folderFlag = 1;
    }else if((fp = fopen(request.url, "rb"))==NULL){
	response.status = 404;
	fp = fopen("404.html","rb");
#ifdef DEBUG
	fprintf(stderr, "404 error occured");
#endif
	/* clear the response 's error type and replace it to default value text/html */
	memset(response.contentType, '\0', sizeof(response.contentType));
	strcpy(response.contentType, "text/html");
    }else{
	/* open the path file successfully */
	response.status = 200;
	if(parseRequest(buf) == 2)
	    cgiFlag = 1;
#ifdef DEBUG
	fprintf(stderr, "200 ok, get the file");
#endif
    }

    /* open file finished */
    int len, headLen, len1, len2;
    char folderBuf[100000];
    if(fp){
	memset(content, '\0', BUF2_SIZE);
	memset(folderBuf, '\0', 100000);
	memset(buf, '\0', BUF_SIZE);

	if(folderFlag == 1){
	    strcpy(content, "<html><body><table><tr><a>Name</a></tr><tr><td valign='top'></td><td><a href='/'>Parent Directory/</a></td>");
	    len = strlen(content);
	    char * item = "<tr><td valign='top'></td><td><a href='/%s'>%s</a></td>";
	    char item_tmp[100];
	    char * ender = "</table></body></html>";

	    /* len = fread(content, sizeof(unsigned char), BUF2_SIZE, fp); */

	    for(struct fileList* p=node;p->p_next;p=p->p_next){
		fprintf(stderr, "%s\n", p->name);
	    }

	    do{
		fprintf(stderr, "start to print a node");
		fprintf(stderr, "%s\n", node->name);
		sprintf(item_tmp, item, node->name, node->name);
		node = node->p_next;
		strcpy(content + len, item_tmp);
		len = strlen(content);
	    }while(node->p_next);
	    strcpy(content + len, ender);
	    len = strlen(content);
	    /* sprintf(folderBuf, content, "index.html"); */
	    fprintf(stderr, "%s", content);
	    headLen = get_head(buf, len);
	}else if(cgiFlag == 0){
	    len =fread(content, sizeof(unsigned char), BUF2_SIZE, fp);
	    headLen = get_head(buf, len);
	}else{
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
	}
    }else{
	fp = fopen("index.html", "rb");
	len = fread(content, sizeof(unsigned char), BUF2_SIZE, fp);
	headLen = get_head(buf, len);
    }
    /* then it is time to merge the http-header and http-content */
    /* do not use strcpy cause the exisience of \0 */

    int i=0;
    for(i=0; i<len; i++){
	buf[headLen + i] = content[i];
	if((i+headLen) == BUF_SIZE){
	    fprintf(stderr, "the content length is beyond the limitation");
	}
    }
    fprintf(stderr, "%d\n", len);
    fprintf(stderr, "%d\n", headLen);
    fprintf(stderr, "%s\n", buf);

    return len+headLen;

}

/* parse http request and pass it to struct */
int parseRequest(unsigned char* buf){
    memset(request.url, '\0', URL_MAX_LEN);
    *(request.url) = '.';
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
	/* read the file path which will be open later */
	for(i=4; i< BUF2_SIZE; i++){
	    if(buf[i]!=' '){
		{
		    *bp = buf[i];
		    ++bp;
		    if(flag == 1){
			*extp = buf[i];
			++extp;
		    }
		}
		if(buf[i]=='.'){
		    flag = 1;
		}
	    
	    }else{
		/* read file finished ! */
		if(flag==1){
		    fprintf(stderr, "the open file path is :%s \n", request.url);
		    fprintf(stderr, "the ext is :%s \n", ext);
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
		    /* return 1 means the request.url is a folder */
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
    fprintf(stderr, "unknown error");
    return -1;
}

/* generate header and return the length of it  */
int get_head(unsigned char *buf, int contentLen){
    memset(buf, '\0', sizeof(buf));
    strcpy((char*)buf, httpVersion);
    int len=strlen((char*)buf);
    if(response.status==200)strcpy((char*)(buf+len), " 200 OK\r\n");
    else if(response.status==500)strcpy((char*)(buf+len), " 500 Internal Server Error\r\n");
    else if(response.status==404)strcpy((char*)(buf+len), " 404 Not Found\r\n");

    /* the original length plus the string length of response status */
    len += strlen((char*)(buf+len));
    sprintf((char*)(buf+len), "Server: %s\r\nContent-Length: %d\r\nContent-Type: %s; charset=UTF-8\r\n\r\n", serverName, contentLen, response.contentType);
    return len+strlen((char*)(buf+len));
}


struct fileList * listFiles(const char *basepath){
    /* memset(allFile, '\0', strlen(allFile)); */
    /* char path[1000]; */
    /* char tmp_name[1000]; */
    struct dirent *dp;
    DIR *dir = opendir(basepath);

    struct fileList * node;
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
	    /* strcpy(path, basepath); */
	    /* strcat(path, "/"); */
	    /* strcat(path, dp->d_name); */
	    /* strcpy(tmp_name, dp->d_name); */
	    /* strcat(tmp_name, "\n"); */
	    /* strcpy(allFile+len, tmp_name); */
	    /* len = strlen(allFile); */
	    /* listFiles(path); */
	}
    }
    closedir(dir);
    return nodeHead;
}

