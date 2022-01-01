/*16104402     */
/*Mumtaz DANACI*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <sys/sysmacros.h>

static void signalCatcher(int sig);
void myfilerecursive(char *basePath, char path[]);
void printMyPath(char paths[]);
int fileNameControl(char arr[]);
int fileTypeControl(char arr[]);
int permissionControl(char arr[]);
int permControl(char arr[]);
int linkControl(char arr[]);
int sizeControl(char arr[]);
int check (char arr[]);
	
	char* targetName = NULL;
	char* fileName = NULL;
	char* fileSize = NULL;
	char* fileType = NULL;
	char* permission = NULL;
	char* numOfLink = NULL;

	int targetNameFlag = 0;
	int fileNameFlag = 0;
	int fileSizeFlag = 0;
	int fileTypeFlag = 0;
	int	permissionFlag = 0;
	int numOfLinkFlag = 0;

int main (int argc, char **argv)
{
	signal(SIGINT,signalCatcher);

    char paths[1024] = {};
	
	int c;

	/*Checking the parameters and if necessary showing the usage.*/
	while ((c = getopt (argc, argv, "w:f:b:t:p:l:")) != -1){

		if(c == 'w'){
			targetName = optarg;
			
		}
		else if(c == 'f'){
			fileName = optarg;
			
		}
		else if(c == 'b'){
			fileSize = optarg;
			
		}
		else if(c == 't'){
			fileType = optarg;
			
		}
		else if(c == 'p'){
			permission = optarg;
			
		}
		else if(c == 'l'){
			numOfLink = optarg;
			
		}

	}

	if(permission != NULL && strlen(permission) != 9){
		printf("Permission must be 9 characters!\n");
		exit(EXIT_FAILURE);
	}
	myfilerecursive(targetName, paths);
	strcat(paths,"\0");
	printMyPath(paths);


	return 0;
}
static void signalCatcher(int sig)
{
	switch(sig) {
		case SIGINT:
			printf("\nCaught a signal: CTRL-C \n");
			exit(1);
			break;
	}
}
void printMyPath(char paths[]){

	int slash = 0;
	
	for (int i = 0; paths[i] != '\0' ; i++)
	{
		if(paths[i] == '\n'){
			slash = 0;
			printf("\n");

		}else if(paths[i] == '/'){
			
			printf("\n|" );
			for (int j = 0; j < slash+1; j++)
			{
				printf("--");
			}

			slash++;
		}else{
			printf("%c",paths[i] );
		}
	
	}

}

void myfilerecursive(char *basePath, char paths[])
{
	int control = 0;
	char path[200];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
   	
    if (!dir){
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
    
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        	
			strcpy(path, basePath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			control = check(path);
		
			if (control == 1)
			{
				strcat(paths, path);
				strcat(paths,"\n");   	
			}       
                    
            myfilerecursive(path, paths);
     
        }
    }

    closedir(dir);
}

int check (char arr[]){


	if (fileName != NULL)
	{
		fileNameFlag = fileNameControl(arr);
	}
	if (fileType != NULL)
	{
		fileTypeFlag = fileTypeControl(arr);
	}
	if (fileSize != NULL)
	{
		fileSizeFlag = sizeControl(arr);
	}
	if (permission != NULL)
	{
		permissionFlag = permissionControl(arr);
	}
	if (numOfLink != NULL)
	{
		numOfLinkFlag = linkControl(arr);
	}


	if(fileNameFlag != -1 && fileTypeFlag != -1 && fileSizeFlag != -1 && permissionFlag != -1 && numOfLinkFlag != -1){
		return 1;
	}else return -1;
}
int fileNameControl(char arr[]){
	if(strstr(arr,fileName) != NULL){
		return 1;
	}else return -1;
}
int fileTypeControl(char arr[]){
	struct stat sb;
	char* flag;
        
	if (lstat(arr, &sb) == -1) {
	   perror("lstat");
	   exit(EXIT_FAILURE);
	}

	switch (sb.st_mode & S_IFMT) {
		case S_IFBLK:  flag = "b";            break;
		case S_IFCHR:  flag = "c";        	  break;
		case S_IFDIR:  flag = "d";            break;
		case S_IFIFO:  flag = "p";            break;
		case S_IFLNK:  flag = "l";            break;
		case S_IFREG:  flag = "f";            break;
		case S_IFSOCK: flag = "s";            break;
		default:       flag = "u";            break;
	}

	if(!strcmp(fileType,flag)){
		return 1;
	}else return -1;

}

int permissionControl(char arr[]){
	struct stat sb;

	if (lstat(arr, &sb) == -1) {
	   perror("lstat");
	   exit(EXIT_FAILURE);
	}

	char perm1[4] ;	 
	char perm2[4] ;	
	char perm3[4] ;

	perm1[0] = permission[0];
	perm1[1] = permission[1];
	perm1[2] = permission[2];
	perm1[3] = '\0';

	perm2[0] = permission[3];
	perm2[1] = permission[4];
	perm2[2] = permission[5];
	perm2[3] = '\0';

	perm3[0] = permission[6];
	perm3[1] = permission[7];
	perm3[2] = permission[8];
	perm3[3] = '\0';
	
	int permNum1 = permControl(perm1);
	int permNum2 = permControl(perm2);
	int permNum3 = permControl(perm3);

	int a = (uintmax_t) sb.st_mode%8;
	int b = ((uintmax_t) sb.st_mode/8)%8;
	int c = ((uintmax_t) sb.st_mode/64)%8;

	if(a == permNum3 && b == permNum2 && c == permNum1){
		return 1;
	}else return -1;
           
}
int permControl(char arr[]){
	
	if(!strcmp(arr,"---")){
		return 0;
	}else if(!strcmp(arr,"--x")){
		return 1;
	}else if(!strcmp(arr,"-w-")){
		return 2;
	}else if(!strcmp(arr,"-wx")){
		return 3;
	}else if(!strcmp(arr,"r--")){
		return 4;
	}else if(!strcmp(arr,"r-x")){
		return 5;
	}else if(!strcmp(arr,"rw-")){
		return 6;
	}else if(!strcmp(arr,"rwx")){
		return 7;
	}else return -1;
}
int linkControl(char arr[]){
	struct stat sb;

	if (lstat(arr, &sb) == -1) {
	   perror("lstat");
	   exit(EXIT_FAILURE);
	}
	
	if(atoi(numOfLink) == (uintmax_t) sb.st_nlink){
		return 1;
	}else return -1;
}
int sizeControl(char arr[]){
	struct stat sb;

	if (lstat(arr, &sb) == -1) {
		perror("lstat");
		exit(EXIT_FAILURE);
	}

	if(atoi(fileSize) == (intmax_t) sb.st_size){
		return 1;
	}else return -1;
}