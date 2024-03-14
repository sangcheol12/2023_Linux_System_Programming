#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RW_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/*
umask : 프로세스의 파일 생성 마스크를 생성하고 이전 파일 모드 값을 리턴
*/
int main(void){
	char *fname1="ssu_file1";
	char *fname2="ssu_file2";

	umask(0);

	if(creat(fname1,RW_MODE)<0){
		fprintf(stderr,"creat error for %s\n",fname1);
		exit(1);
	}

	umask(S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    // (S_IRGRP : 그룹 사용자의 읽기 권한을 지정) / (S_IWGRP : 그룹 사용자의 쓰기 권한을 지정) / (S_IROTH : 다른 사용자의 읽기 권한을 지정) / (S_IWOTH ㅣ 다른 사용자의 쓰기 권한을 지정)

	if(creat(fname2,RW_MODE)<0){
		fprintf(stderr,"creat error for %s\n", fname2);
		exit(1);
	}
	exit(0);
}