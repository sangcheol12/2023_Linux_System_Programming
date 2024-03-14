#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

int main(void){
	char buf[BUFFER_SIZE];
	char *fname="ssu_test.txt";
	int fd;
	int length;

	if((fd=open(fname,O_RDONLY,0644))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	if(dup2(1,4)!=4){ // 1번 파일 디스크립터(표준 출력을) 4번으로 복사 -> 4번 파일 디스크립터를 쵸준 출력 처럼 쓸 수 있음
		fprintf(stderr,"dup2 call failed\n");
		exit(1);
	}

	while(1){
		length=read(fd,buf,BUFFER_SIZE);

		if(length<=0)
			break;
		write(4,buf,length);
	}
	exit(0);
}