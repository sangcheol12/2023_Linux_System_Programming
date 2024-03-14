#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
chdir : 현재 작업 디렉토리를 변경
성공 시 0, 실패 시 -1 리턴
*/
int main(void){
	if(chdir("/etc")<0){
		fprintf(stderr,"chdir error\n");
		exit(1);
	}
	printf("chdir to /etc succeeded.\n");
	exit(0);
}