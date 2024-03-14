#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#ifdef PATH_MAX
static int pathmax=PATH_MAX;
#else
static int pathmax=0;
#endif

#define MAX_PATH_GUESSED 1024

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

char *pathname;
char command[LINE_MAX],grep_cmd[LINE_MAX];

int ssu_do_grep(void){
	struct dirent *dirp;
	struct stat statbuf;
	char *ptr;
	DIR *dp;

	if(lstat(pathname,&statbuf)<0){
		fprintf(stderr,"lstat error for %s\n",pathname);
		return 0;
	}

	if(S_ISDIR(statbuf.st_mode)==0){ // 스탯으로 들어온게 디렉토리가 아니라면 command에 명령어 복사 후 system 함수로 grep 명령어 실행
		sprintf(command,"%s %s",grep_cmd,pathname);
		printf("%s : \n",pathname);
		system(command);
		return 0;
	}

	ptr=pathname+strlen(pathname);
	*ptr++='/';
	*ptr='\0';

	if((dp=opendir(pathname))==NULL){ // 디렉토리를 열고
		fprintf(stderr,"opendir error for %s\n",pathname);
		return 0;
	}

	while((dirp=readdir(dp)) !=NULL) // 오픈 된 디렉토리 하위 파일들 읽음
		if(strcmp(dirp->d_name, ".") && strcmp(dirp->d_name,"..")) {
			strcpy(ptr, dirp->d_name);

			if(ssu_do_grep()<0) // 재귀적으로 반복
				break;
		}

	ptr[-1]=0;
	closedir(dp);
	return 0;
}

/*
grep -n Keyword <경로> 형식으로 문자열 생성
*/
void ssu_make_grep(int argc,char *argv[]){
	int i;
	strcpy(grep_cmd, "grep");

	for(i=1;i<argc-1;i++){
		strcat(grep_cmd," ");
		strcat(grep_cmd,argv[i]);
	}
}

int main(int argc, char *argv[]){
	if(argc<2){
		fprintf(stderr,"usage: %s <-CVbchilnsvwx> <-num> <-A num> <-B num> <-f file> \n",argv[0]);
		exit(1);
	}

	if(pathmax==0){
        /*
        Path 매개변수에 의해 이름 지정된 파일에 포함된 파일 시스템이 지원하는 조작의 특성을 판별
        PC_PATH_MAX : Path 매개변수에 의해 이름 지정된 파일에 포함된 파일 시스템이 지원하는 조작의 특성을 판별
        */
		if((pathmax=pathconf("/",_PC_PATH_MAX))<0)
			pathmax=MAX_PATH_GUESSED;
		else
			pathmax++;
	}

	if((pathname=(char *) malloc(pathmax+1))==NULL){
		fprintf(stderr,"malloc error\n");
		exit(1);
	}

	strcpy(pathname,argv[argc-1]);
	ssu_make_grep(argc,argv);
	ssu_do_grep();
	exit(0);
}