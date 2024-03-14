#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	struct stat file_info;
	char *str;
	int i;

	for(i=1;i<argc;i++){
		printf("name = %s, type = ",argv[i]);
		if(lstat(argv[i],&file_info)<0){ // link stat : 성공 시 0, 실패 시 -1 반환
			fprintf(stderr,"lstat error\n");
			continue;
		}
		if(S_ISREG(file_info.st_mode)) // 일반 파일
			str="regrular";
		else if(S_ISDIR(file_info.st_mode)) // 디레토리 파일
			str="directory";
		else if(S_ISCHR(file_info.st_mode)) // 문자 특수 파일
			str="character special";
		else if(S_ISBLK(file_info.st_mode)) // 블록 특수 파일
			str="block special";
		else if(S_ISFIFO(file_info.st_mode)) // FIFO(프로세스 통신에 쓰이는 파일)
			str="FIFO";
		else if(S_ISLNK(file_info.st_mode)) // 심볼릭 링크
			str="symbolic link";
		else if(S_ISSOCK(file_info.st_mode)) // 소켓(프로세스 네트워크 통신에 쓰이는 파일)
			str="socket";
		else
			str="unknown mode";
		printf("%s\n",str);
	}
	exit(0);
}

