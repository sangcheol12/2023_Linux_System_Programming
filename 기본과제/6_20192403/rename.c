#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
파일이나 디렉토리의 이름을 변경하는 시스템 호출
성공 시 0, 실패 시 -1 리턴
*/
int main(int argc, char *argv[]){
	int fd;
	if(argc!=3){
		fprintf(stderr,"usage: %s <oldname> <newname>\n",argv[0]);
		exit(1);
	}
	if((fd=open(argv[1],O_RDONLY))<0){ // argv[1] 이름의 파일 오픈
		fprintf(stderr,"first open error for %s\n",argv[1]);
		exit(1);
	}
	else
		close(fd);

	if(rename(argv[1],argv[2])<0){ // argv[2]의 이름으로 파일명 변경
		fprintf(stderr,"rename error\n");
		exit(1);
	}
	
	if((fd=open(argv[1],O_RDONLY))<0) // 위에서 파일명을 바꿨으므로 안열림
		printf("second open error for %s\n",argv[1]);
	else{
		fprintf(stderr,"it's very strange!\n");
		exit(1);
	}

	if((fd=open(argv[2],O_RDONLY))<0){ // argv[2] 이름의 파일 오픈
		fprintf(stderr, "third open error for %s\n",argv[2]);
		exit(1);
	}
	printf("Everything is good!\n");
	exit(0);
}