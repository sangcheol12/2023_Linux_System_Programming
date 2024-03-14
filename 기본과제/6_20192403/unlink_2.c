#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
	char buf[64];
	char *fname="ssu_tempfile";
	int fd;
	int length;
	if((fd=open(fname, O_RDWR|O_CREAT|O_TRUNC,0600))<0){ // 사용자에게 읽고 쓰기기 권한이 있는 파일 생성
		fprintf(stderr,"first open error for %s\n",fname);
		exit(1);
	}
	if(unlink(fname)<0){ // 링크 해제
		fprintf(stderr,"unlink error for %s\n",fname);
		exit(1);
	}
	if(write(fd,"How are you?",12)!=12){ // 파일에 12바잍 작성
		fprintf(stderr,"write error\n");
		exit(1);
	}
	lseek(fd,0,0); // 파일의 처음으로 이동
	if((length=read(fd,buf,sizeof(buf)))<0){ // 파일 내용 읽고 버퍼에 저장
		fprintf(stderr,"buf read error\n");
		exit(1);
	}
	else
		buf[length]=0;

	printf("%s\n",buf); // 버퍼 내용 출력

	close(fd);

	if((fd=open(fname,O_RDWR))<0){ // 읽고 쓰기 권한이 있게 파일 생성
		fprintf(stderr,"second oepn error for %s\n",fname);
		exit(1);
	}

	else
		close(fd);

	exit(0);
}