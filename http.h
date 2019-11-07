#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#define BUF2_SIZE 20000000
#define URL_MAX_LEN 8183

#define WORK_DIR "html"
#define GET 1
#define POST 2

int handle_request(unsigned char *);

typedef struct httpRequest{
    int method;
    char url[URL_MAX_LEN];
} httpRequest;

typedef struct httpResponse{
    int status;
    char contentType[30];
    int contentLength;
} httpResponse;

struct fileList{
    char * name;
    struct fileList * p_next;
};

int getContent(FILE* fp);

int getHead(unsigned char* buf, int contentLen);

int parseRequest(unsigned char* buf);

void printDir(char* preDir, int preLen, char* dir, int* len);

void runProcess(int url_len);

int handleRequest(unsigned char* buf);

int get_head(unsigned char* buf, int contentLen);

