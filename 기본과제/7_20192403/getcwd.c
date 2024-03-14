#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 1024

/*
현재 작업 디렉토리의 전체 경로 이름을 얻어오는 시스템 함수
*/
int main(void){
	char *pathname;

	if(chdir("/Users/parksangcheol")<0){ // Users/parksangcheol로 작업 디렉토리 변경
		fprintf(stderr,"chdir error\n");
		exit(1);
	}

	pathname=malloc(PATH_MAX);
	if(getcwd(pathname,PATH_MAX)==NULL){ // 작업 디렉토리 경로 받아옴
		fprintf(stderr,"getcwd error\n");
		exit(1);
	}
	printf("current directory=%s\n",pathname); // 출력
	exit(0);
}