#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
link : 기존 파일에 링크를 생성하는 함수
성공 시 0, 실패 시 -1 리턴
*/
int main(int argc, char *argv[]) {
	if(argc<3){
		fprintf(stderr,"usage: %s <file1> <file2>\n",argv[0]);
		exit(1);
	}
	if(link(argv[1],argv[2])==-1) { // 첫 번째 인자로 들어 온 파일을 두번 쨰 이름으로 링크 생성
		fprintf(stderr,"link error for %s\n",argv[1]);
		exit(1);
	}
	exit(0);
}