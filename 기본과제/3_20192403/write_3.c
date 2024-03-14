#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ssu_employee.h"

/*
.을 입력 받을 때까지 정보를 입력받아 구조체에 담아서 파일에 작성하는 코드
*/
int main(int argc, char *argv[]){
	struct ssu_employee record;
	int fd;

	if(argc<2){
		fprintf(stderr, "usage : %s file\n",argv[0]);
		exit(1);
	}
	if((fd=open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0640))<0){ // 첫 번째 인자로 들어온 파일 쓰기 모드로 오픈
		fprintf(stderr,"open error for %s\n",argv[1]);
		exit(1);
	}

	while(1){
		printf("Enter employee name <SPACE> salary: ");
		scanf("%s",record.name); // 구조체 이름 변수 입력

		if(record.name[0]=='.') // 이름으로 들어 온 값이 .이면 종료
			break;

		scanf("%d",&record.salary); // 구조체 임금 변수 입력
		record.pid = getpid();
		write(fd,(char *)&record, sizeof(record)); // 파일에 구조체 정보 작성
	}
	close(fd);
	exit(0);
}