#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/*
입력 받은 만큼 표준 출력으로 출력하는 코드
*/
int main(void){
	char buf[BUFFER_SIZE];
	int length;

	length=read(0,buf,BUFFER_SIZE);
	write(1,buf,length); // 리턴 값 : 성공 시 기록된 바이트 수, 에러 시 -1
	exit(0);
}