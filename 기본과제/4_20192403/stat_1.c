#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	struct stat statbuf; // stat 구조체 : 파일의 대한 정보를 포함하는 구조체

	if(argc!=2){
		fprintf(stderr,"usage: %s <file>\n",argv[0]);
		exit(1);
	}

	if((stat(argv[1],&statbuf))<0) { // 성공 시 0, 실패 시 -1
		fprintf(stderr,"stat error\n");
		exit(1);
	}

	printf("%s is %ld bytes\n",argv[1],statbuf.st_size);
	exit(0);
}