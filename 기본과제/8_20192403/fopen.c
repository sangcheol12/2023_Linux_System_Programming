#include <stdio.h>
#include <stdlib.h>

/*
파일을 오픈라는 라이브러리 함수
fopen : 성공 시 파일 포인터, 에러 시 NULL을 리턴
*/
int main(void){
	char *fname="ssu_test.txt";
	char *mode="r"; // 읽기전용

	if(fopen(fname,mode)==NULL){ // 파일 오픈
		fprintf(stderr,"fopen error for %s\n",fname);
		exit(1);
	}
	else
		printf("Success!\nFilename: <%s>, mode: <%s>\n",fname,mode);

	exit(0);
}