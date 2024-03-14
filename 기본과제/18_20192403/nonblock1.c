#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void set_flags(int fd, int flags);
void clr_flags(int fd, int flags);

char buf[500000];

int main()
{
    int ntowrite, nwrite;
    char *ptr;

    // 표준 입력에서 버퍼에 데이터를 읽음
    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "reading %d bytes\n", ntowrite);

    // 표준 출력의 플래그를 O_NONBLOCK으로 설정
    set_flags(STDOUT_FILENO, O_NONBLOCK);

    ptr = buf;
    while (ntowrite > 0)
    {
        errno = 0;
        // 버퍼에 있는 데이터를 표준 출력에 쓰기 시도
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

        if (nwrite > 0) {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }
    // 표준 출력의 O_NONBLOCK 플래그를 해제합니다.
    clr_flags(STDOUT_FILENO, O_NONBLOCK);
    exit(0);
}

// 파일 디스크립터의 플래그를 설정하는 함수
void set_flags(int fd, int flags) {
    int val;
    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        fprintf(stderr, "fcntl F_GETFL failed");
        exit(1);
    }

    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0) {
        fprintf(stderr, "fcntl F_SETFL failed");
        exit(1);
    }
}

// 파일 디스크립터의 플래그를 해제하는 함수
void clr_flags(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        fprintf(stderr, "fcntl F_GETFL failed");
        exit(1);
    }

    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0) {
        fprintf(stderr, "fcntl F_SETFL failed");
        exit(1);
    }
}
