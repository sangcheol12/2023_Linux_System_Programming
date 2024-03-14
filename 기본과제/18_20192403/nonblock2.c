#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_CALL 100
#define MAX_BUF 20000

void serror(char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

int main(void)
{
    int nread, nwrite, val, i = 0;
    char *ptr;
    char buf[MAX_BUF];

    int call[MAX_CALL];
    
    // 표준 입력에서 버퍼에 데이터를 읽음
    nread = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes\n", nread);

    // 표준 출력의 플래그를 O_NONBLOCK으로 설정
    if ((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
        serror("fcntl F_GETFL error");
    val |= O_NONBLOCK;
    if (fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
        serror("fcntl F_SETFL error");

    // 버퍼에 있는 데이터를 표준 출력에 쓰기 시도
    for (ptr = buf; nread > 0; i++) {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, nread);

        if (i < MAX_CALL)
            call[i] = nwrite;

        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);
        if (nwrite > 0) {
            ptr += nwrite;
            nread -= nwrite;
        }
    }

    // 표준 출력의 O_NONBLOCK 플래그를 해제
    if ((val = fcntl(STDOUT_FILENO, F_GETFL, 0)) < 0)
        serror("fcntl F_GETFL error");
    val &= ~O_NONBLOCK;
    if (fcntl(STDOUT_FILENO, F_SETFL, val) < 0)
        serror("fcntl F_SETFL error");

    // 쓰기 호출(call) 결과를 출력
    for (i = 0; i < MAX_CALL; i++)
        fprintf(stdout, "call[%d] = %d\n", i, call[i]);

    exit(0);
}
