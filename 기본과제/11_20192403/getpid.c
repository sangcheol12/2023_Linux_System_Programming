#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
pid(프로세스 id)를 반환하는 함수
*/
int main(void){
	printf("Process ID        = %d\n",getpid()); // 사용자의 pid
	printf("Parent process ID   = %d\n",getppid()); // 부모 프로세스의 pid
	printf("Real user ID        = %d\n",getuid()); // 실제 user id
	printf("Effective user ID   = %d\n",geteuid()); // 유효 user id
	printf("Real group ID       = %d\n",getgid()); // 실제 그룹 id
	printf("Effective group ID  = %d\n",getegid()); // 유효 그룹 id
	exit(0);
}