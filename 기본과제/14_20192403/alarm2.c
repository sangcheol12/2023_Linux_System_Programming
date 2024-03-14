#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LINE_MAX 2048

static void ssu_alarm(int signo);

int main()
{
    char buf[LINE_MAX];
    int n;

    if (signal(SIGALRM, ssu_alarm) == SIG_ERR) { // 알람 시그널 핸들러 지정
        fprintf(stderr, "SIGALRM error\n");
        exit(1);
    }
    alarm(10); // 알람 호출 시간 설정

    if((n = read(STDIN_FILENO, buf, LINE_MAX)) < 0) { // 표준 입력
        fprintf(stderr, "read() error\n");
        exit(1);
    }
    alarm(0);
    write(STDOUT_FILENO, buf, n); // 표준 출력
    exit(0);
}

static void ssu_alarm(int signo) {
    printf("ssu_alarm() called\n");
}