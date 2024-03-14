#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/*
읽기 전용으로 파일을 열고 파일 디스크립터 복사해서 각 디스크립터로 출력
*/
int main(void){
	char buf[BUFFER_SIZE];
	char *fname="ssu_test.txt";
	int fd1,fd2;
	int count;

	if((fd1=open(fname,O_RDONLY,0644))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	fd2=dup(fd1); // 리턴 값 : 성공 시 새로운 파일 디스크립터, 에러 시 -1 리턴
	count=read(fd1,buf,12); // fd1으로 12 바이트 읽기
	buf[count]=0;
	printf("fd1's printf : %s\n",buf);
	lseek(fd1,1,SEEK_CUR);
	count=read(fd2,buf,12); // fd2로 뒤에 12 바이트 읽기
	buf[count]=0;
	printf("fd2's printf : %s\n",buf);
	exit(0);
}