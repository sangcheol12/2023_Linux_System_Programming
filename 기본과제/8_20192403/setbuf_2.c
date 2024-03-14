#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

/*
표준 입력의 버퍼를 buf로 지정하여 입력을 받고 한글자씩 출력하는 프로그램
*/
int main(void){
	char buf[BUFFER_SIZE];
	int a,b;
	int i;
	setbuf(stdin,buf); // 표준 입력 버퍼 지정
	scanf("%d %d",&a,&b);
	for(i=0;buf[i]!='\n';i++)
		putchar(buf[i]);
	putchar('\n');
	exit(0);
}