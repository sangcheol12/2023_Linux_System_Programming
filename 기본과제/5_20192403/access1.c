#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
access : 파일의 접근 권한을 확인 / 성공시 0, 실패시 -1 리턴
*/
int main(int argc, char *argv[]){
	int i;
	
	if(argc<2){
		fprintf(stderr, "usage: %s <file1> <file2> .. <fileN>\n",argv[0]);
		exit(1);
	}

	for(i=1;i<argc;i++){
		if(access(argv[i],F_OK)<0){ // F_OK 파일의 존재 여부 판정
			fprintf(stderr,"%s doesn't exist.\n",argv[i]);
			continue;
		}
		if(access(argv[i],R_OK)==0) // 읽기 권한 판정
			printf("User can read %s\n",argv[i]);
		if(access(argv[i],W_OK)==0) // 쓰기 권한 판정
			printf("User can write %s\n",argv[i]);
		if(access(argv[i],X_OK)==0) // 실행 권한 판정
			printf("User can execute %s\n",argv[i]);
	}
	exit(0);
}