#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
    int fd;
    int val;

    // "exec_copy.txt" 파일을 생성하고 파일 디스크립터를 얻음
    if ((fd = open("exec_copy.txt", O_CREAT)) < 0) {
        fprintf(stderr, "open error for %s\n", "exec_copy.txt");
        exit(1);
    }

    // 파일 디스크립터의 close-on-exec 플래그 값을 가져옴
    val = fcntl(fd, F_GETFD, 0);

    // close-on-exec 플래그 상태를 출력
    if (val & FD_CLOEXEC)
        printf("close-on-exec bit on\n");
    else
        printf("close-on-exec bit off\n");

    // close-on-exec 플래그를 설정
    val |= FD_CLOEXEC;

    // close-on-exec 플래그 상태를 출력
    if (val & FD_CLOEXEC)
        printf("close-on-exec bit on\n");
    else
        printf("close-on-exec bit off\n");

    // 파일 디스크립터에 변경된 close-on-exec 플래그 값을 설정
    fcntl(fd, F_SETFD, val);

    // "loop" 프로그램을 실행
    execl("/home/parksangcheol/loop", "./loop", NULL);
}
