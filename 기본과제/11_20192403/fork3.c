#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void ssu_charatatime(char *str);

int main(void){
	pid_t pid;

	if((pid=fork())<0){ // 자식 프로세스를 생성하고 에러 처리
		fprintf(stderr,"fork error\n");
		exit(1);
	}
	else if(pid==0) // 자식 프로세스
		ssu_charatatime("output from child\n");
	else
		ssu_charatatime("output from parent\n");

	exit(0);
}

static void ssu_charatatime(char *str){
	char *ptr;
	int print_char;

	setbuf(stdout,NULL); // 버퍼링을 안하도록 설정
	
	for(ptr=str; (print_char=*ptr++)!=0;){ // 10초마다 문자 출력
		putc(print_char,stdout);
		usleep(10);
	}
}