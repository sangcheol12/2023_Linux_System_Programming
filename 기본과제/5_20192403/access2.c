#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define TABLE_SIZE (sizeof(table)/sizeof(*table))

/*
파일의 존재 여부, 살행, 쓰기, 읽기 가능 여부를 table 구조체에 저장하고 access를 이용하여 파일 접근 권한을 확인
*/
int main(int argc, char *argv[]){
	struct{
		char *text;
		int mode;
	}table[]={
		{"exists",0},
		{"execute",1},
		{"write",2},
		{"read",4}
	};

	int i;
	if(argc<2){
		fprintf(stderr,"usage : %s <file>\n",argv[0]);
		exit(1);
	}

	for(i=0;i<TABLE_SIZE;i++){
		if(access(argv[1],table[i].mode)!=-1)
			printf("%s -ok\n",table[i].text);
		else
			printf("%s\n",table[i].text);
	}
	exit(0);
}