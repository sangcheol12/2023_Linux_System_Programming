#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define S_MODE 0644
#define BUFFER_SIZE 1024

/*
인자로 들어온 파일을 열어서 두번 째 인자로 들어온 파일에 작성하는 코드
*/
int main(int argc,char *argv[]){
	char buf[BUFFER_SIZE];
	int fd1,fd2;
	int length;

	if(argc!=3){ // 인자가 3개가 아니면 에러
		fprintf(stderr,"Usage : %s filein fileout\n",argv[0]);
		exit(1);
	}
	if((fd1=open(argv[1],O_RDONLY))<0){ // 첫 인자로 들어온 파일 읽기 모드로 오픈
		fprintf(stderr,"open error for %s\n",argv[1]);
		exit(1);
	}
	if((fd2=open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,S_MODE))<0){  // 두번 째 인자로 들어온 파일 쓰기 모드로 오픈
		fprintf(stderr,"open error for %s\n",argv[2]);
		exit(1);
	}
	while((length=read(fd1,buf,BUFFER_SIZE))>0) // 첫 인자의 파일 내용을 최대 1024까지 읽고 두 번째 인자로 들어온 파일에 쓰기
		write(fd2,buf,length);
	exit(0);
}