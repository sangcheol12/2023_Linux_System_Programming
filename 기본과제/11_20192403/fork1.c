#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char glob_str[]="write to standard output\n";
int glob_val=10;

int main(void){
	pid_t pid;
	int loc_val;
	loc_val=100;
	if(write(STDOUT_FILENO,
				glob_str,sizeof(glob_str)-1)!=sizeof(glob_str)-1){
		fprintf(stderr,"write error\n");
		exit(1);
	}

	printf("before fork\n");

	if((pid=fork())<0){ // 자식 프로세스 생성
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if(pid==0){ // 자식 프로세스에서 실행할 코드
		glob_val++;
		loc_val++;
	}
	else // 부모 프로세스에서 실행할 코드
		sleep(3);

	printf("pid = %d, glob_val = %d, loc_val = %d\n",getpid(),glob_val,loc_val); //프로세스 ID, 전역 변수, 지역 변수 출력 후 프로그램 종료
	exit(0);
}