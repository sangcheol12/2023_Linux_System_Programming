#include <stdio.h>
#include <stdlib.h>

/*
exit과 _exit의 차이
exit은 라이브러리 함수로 버퍼를 비우고 종료
_exit은 시스템 콜 함수로 버퍼를 비우지 않고 종료
*/
int main(void){
	printf("Good afternoon");
	exit(0);
}