#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_signal_handler(int signo);

int count = 0;

int main(void)
{
    signal(SIGALRM,ssu_signal_handler); // 지정 시간 마다 알람 핸들러로 출력
    alarm(1); // 알람 호출 시간 설정

    while(1);

    exit(0);
}

void ssu_signal_handler(int signo) {
    printf("alarm %d\n", count++);
    alarm(1);
}