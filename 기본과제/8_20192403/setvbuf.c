#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void ssu_setbuf(FILE *fp,char *buf);

int main(void){
	char buf[BUFFER_SIZE];
	char *fname="/dev/ttys000"; // 현재 버퍼
	FILE *fp;

	if((fp=fopen(fname,"w"))==NULL){ // 버퍼 오픈
		fprintf(stderr,"fopen error for %s",fname);
		exit(1);
	}

	ssu_setbuf(fp,buf); // buf로 버퍼 지정
	fprintf(fp,"Hello, "); sleep(1);
	fprintf(fp,"UNIX!!"); sleep(1);
	fprintf(fp,"\n"); sleep(1);
	ssu_setbuf(fp,NULL); // 버퍼 해제
	fprintf(fp,"HOW"); sleep(1);
	fprintf(fp," ARE"); sleep(1);
	fprintf(fp," YOU?"); sleep(1);
	fprintf(fp,"\n"); sleep(1);
	exit(0);
}

void ssu_setbuf(FILE *fp,char *buf){
	size_t size;
	int fd;
	int mode;
	fd=fileno(fp);
	if(isatty(fd)) // 파일 지정자 desc가 터미널에 연결되어 있는지 확인
		mode=_IOLBF;
	else
		mode=_IOFBF;
	if(buf==NULL){
		mode=_IONBF;
		size=0;
	}
	else
		size=BUFFER_SIZE;
	setvbuf(fp,buf,mode,size);
}