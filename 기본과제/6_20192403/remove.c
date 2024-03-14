#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
remove 파일이나 디렉토리를 unlink하는 시스템 호출
성공 시 0, 실패 시 -1 리턴
*/
int main(int argc,char *argv[]) {
	if(argc!=3){
		fprintf(stderr,"usage: %s <oldname> <newname>\n",argv[0]);
		exit(1);
	}
	if(link(argv[1],argv[2])<0){ // 파일 링크로 argv[2]이름으로 파일 생성
		fprintf(stderr,"link error\n");
		exit(1);
	}
	else
		printf("step1 passed.\n");

	if(remove(argv[1])<0){ // argv[1]이름의 파일 삭제
		fprintf(stderr,"remove error\n");
		remove(argv[2]); // 삭제에 실패하면 에러 출력 후 argv[2]파일 삭제
		exit(1);
	}
	else
		printf("step2 passed.\n");

	printf("Success!\n");
	exit(0);
}