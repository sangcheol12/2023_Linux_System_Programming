#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

/*
chmod 파일의 접근 권한을 변경해주는 함수
성공 시 0, 에러 시 -1 리턴
*/
int main(void){
	struct stat statbuf;
	char *fname1="ssu_file1";
	char *fname2="ssu_file2";

	if(stat(fname1,&statbuf)<0)
		fprintf(stderr,"stat error %s\n",fname1);

	if(chmod(fname1,(statbuf.st_mode & ~S_IXGRP)|S_ISUID)<0) // S_IXGRP : 그룹 사용자의 실행 권한 지정
		fprintf(stderr,"chmod error %s\n",fname1);

    /*
    S_IRUSR : 소유자의 읽기 권한 지정
    S_IWUSR : 소유자의 쓰기 권한 지정
    S_IRGRP : 그룹 사용자의 읽기 권한 지정
    S_IROTH : 다른 사용자의 읽기 권한 지정
    S_IXOTH : 다른 사용자의 실행 권한 지정
    */
	if(chmod(fname2,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IXOTH)<0)
		fprintf(stderr,"chmod error %s\n",fname2);

	exit(0);
}