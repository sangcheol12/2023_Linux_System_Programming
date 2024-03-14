#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void ssu_alarm(int signo);

int main(void)
{
    printf("Alarm Setting\n");
    signal(SIGALRM, ssu_alarm); // 시그널 핸들러 지정
    alarm(2);

    while (1) {
        printf("done\n");
        pause(); // 시그널 수신 대기 상태
        alarm(2);
    }
    exit(0);
}

void ssu_alarm(int signo) {
    printf("alarm..!!!\n");
}