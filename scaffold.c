#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

void listFiles(const char *path);
char allFile[10000] = {'\0'};
int len = 0;

int main(){
    char path[100];

    printf("Enter path to scan:");
    scanf("%s", path);

    listFiles(path);
    printf("%s\n", allFile);
    return 0;
}

void listFiles(const char *basepath){
    char path[1000];
    char tmp_name[1000];
    struct dirent *dp;
    DIR *dir = opendir(basepath);

    if(!dir)
	return;
    while((dp = readdir(dir))!=NULL){
	if(strcmp(dp->d_name,".") != 0 && strcmp(dp->d_name, "..") != 0){
	    /* printf("%s\n",dp->d_name); */
	    strcpy(path, basepath);
	    strcat(path, "/");
	    strcat(path, dp->d_name);
	    strcpy(tmp_name, dp->d_name);
	    strcat(tmp_name, "\n");
	    strcpy(allFile+len, tmp_name);
	    len = strlen(allFile);
	    listFiles(path);
	}
    }

    closedir(dir);
}
