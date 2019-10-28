#include <string.h>
#include "http.h"
#include "action.h"
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

const char * httpVersion="HTTP/1.1";
const char * serverName="Kapache";

static httpRequest request;
static httpResponse response;

static unsigned char content[BUF2_SIZE];
static unsigned char url_parse[20];


int handleRequest(unsigned char* buf){
#ifdef DEBUG
    fprintf(stderr, "request:\n %s", buf);
#endif
    FILE * fp;
    if(parseRequest(buf) == -1){
	response.status = 500;
	fp = fopen("500.html", "rb");
#ifdef DEBUG
	fprintf(stderr, "500 error occured");
#endif
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
#ifdef DEBUG
	fprintf(stderr, "200 ok, get the file");
#endif
    }
    /* open file finished */
    int len, headLen;
    if(fp){
	memset(content, '\0', BUF2_SIZE);
	len =fread(content, sizeof(unsigned char), BUF2_SIZE, fp);
	headLen = get_head(buf, len);
	
#ifdef DEBUG
	fprintf(stderr, "\ncontentlength:%d\n",len);
	fprintf(stderr, "\nheadlength:%d\n",headLen);
#endif
    }else{
	fp = fopen("index.html", "rb");
	len = fread(content, sizeof(unsigned char), BUF2_SIZE, fp);
	headLen = get_head(buf, len);
    }
    /* then it is time to merge the http-header and http-content */
    /* do not use strcpy cause the exisience of \0 */
    fprintf(stderr, "%d\n", len);
    fprintf(stderr, "%d\n", headLen);
    fprintf(stderr, "%s\n", buf);

    int i=0;
    for(i=0; i<len; i++){
	buf[headLen + i] = content[i];
	if((i+headLen) == BUF_SIZE){
	    fprintf(stderr, "the content length is beyond the limitation");
	}
    }
    fprintf(stderr, "response:\n%s \n",buf);
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
		    else strcpy(response.contentType, "text/html");
		}else{
		    fprintf(stderr, "the open folder path is:%s \n",request.url);
		    strcpy(response.contentType, "text/html");
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

/* GET / HTTP/1.1 */
/* Host: 127.0.0.1:12334 */
/* User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:70.0) Gecko/20100101 Firefox/7 */
/* 0.0 */
/* Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*[>;q=0.8 */
/* Accept-Language: en-US,en;q=0.5 */
/* Accept-Encoding: gzip, deflate */
/* Connection: keep-alive */
/* Upgrade-Insecure-Requests: 1 */
