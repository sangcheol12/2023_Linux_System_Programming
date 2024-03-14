#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

/*
표준 출력의 버퍼를 바꿔가며 출력을 진행하는 함수
Hello OSLAB!!은 개행을 만날 때 한번에 출력 뒤에 단어는 1초마다 출력 됨
setbuf : 성공 시 0, 에러 시 0이 아닌 값을 리턴
*/
int main(void) {
	char buf[BUFFER_SIZE];
	setbuf(stdout,buf); // 표준 출력의 버퍼를 1024 크기로 지정
	printf("Hello, ");
	sleep(1);
	printf("OSLAB!!");
	sleep(1);
	printf("\n");
	sleep(1);
	setbuf(stdout,NULL); // NULL로 표준 출력의 버퍼 해제
	printf("HOW");
	sleep(1);
	printf(" are");
	sleep(1);
	printf(" you?");
	sleep(1);
	printf("\n");
	exit(0);
}