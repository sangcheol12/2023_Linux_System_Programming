#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/*
파일 디스크립터에 값을 표준 출력으로 복사하는 코드
*/
int main(void){
	char *fname="ssu_test.txt";
	int fd;

	if((fd=creat(fname,0666))<0){ // 읽기 전용으로 오픈
		printf("creat error for %s\n",fname);
		exit(1);
	}

	printf("First printf is on the screen.\n");
	dup2(fd,1); // fd 값을 1(표준 출력으로 복사)
	printf("Second printf is in this file.\n"); // fd 파일이 표준 출력이 되어 이 글이 파일 안으로 들어감
	exit(0);
}