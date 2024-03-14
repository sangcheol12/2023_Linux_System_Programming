
 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
파일 소유자 ID와 그룸 사용자 ID를 변경하는 함수
성공 시 0, 실패 시 -1 리턴
*/
int main(void){
	struct stat statbuf;
	char *fname="ssu_myfile";
	int fd;
	if((fd=open(fname,O_RDWR|O_CREAT,0600))<0){
		fprintf(stderr,"open error for %s\n",fname);
		exit(1);
	}
	close(fd);
	stat(fname,&statbuf);
    // UID : 사용자명에 상응하는 번호, GID : 그룹명에 상응하는 번호
	printf("# 1st stat call # UID:%d   GID:%d\n",statbuf.st_uid,statbuf.st_gid);

	if(chown(fname,501,300)<0){
		fprintf(stderr,"chown error for %s\n",fname);
		exit(1);
	}

	stat(fname,&statbuf);
	printf("# 2nd stat call # UID:%d   GID:%d\n",statbuf.st_uid,statbuf.st_gid);

	if(unlink(fname)<0){
		fprintf(stderr,"unlink error for %s\n",fname);
		exit(1);
	}

	exit(0);
}