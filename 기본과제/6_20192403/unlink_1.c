#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/*
파일의 링크를 해제하는 함수
성공 시 0, 실패 시 -1 리턴
*/
int main(void){
	char *fname="ssu_dump.txt";
	if(open(fname,O_RDWR)<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}
	if(unlink(fname)<0){
		fprintf(stderr,"unlink error for %s\n",fname);
		exit(1);
	}

	printf("File unlinked\n");
	sleep(20);
	printf("Done\n");
	exit(0);
}