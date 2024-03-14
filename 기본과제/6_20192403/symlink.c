#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
심볼릭 링크를 생성하는 시스템 호출
성공 시 0, 실패 시 -1 리턴
*/
int main(int argc, char *argv[]){
	if(argc!=3){
		fprintf(stderr,"usage: %s <actualname> <symname>\n",argv[0]);
		exit(1);
	}

	if(symlink(argv[1],argv[2])<0){ // argv[2] 이름의 심볼릭 링크 생성
		fprintf(stderr,"symlink error\n");
		exit(1);
	}
	else
		printf("symlink: %s -> %s\n",argv[2],argv[1]);
	exit(0);
}