#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/*
freopen으로 처음 오픈한 파일을 표준 출력으로 다시 오픈하는 함수
freopen 성공 시 파일 포인터, 실패 시 NULL 리턴
*/
int main(void) {
	char *fname="ssu_test.txt";
	int fd;

	printf("First printf : Hello, OSLAB!!\n");

	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}

	if(freopen(fname,"w",stdout)!=NULL)
		printf("Second printf : Hello, OSLAB!!\n");

	exit(0);
}