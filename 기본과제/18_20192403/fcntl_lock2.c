#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    struct flock lock;
    int fd;
    char command[100];

    // 명령줄 인수로 전달된 파일을 읽기/쓰기 모드로 오픈
    if ((fd = open(argv[1], O_RDWR)) == -1) {
        perror(argv[1]);
        exit(1);
    }

    // 파일에 쓰기 잠금을 설정
    lock.l_type = F_WRLCK;
    lock.l_whence = 0;
    lock.l_start = 0L;
    lock.l_len = 0L;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        if (errno == EACCES) {
            printf("%s busy -- try later\n", argv[1]);
            exit(2);
        }
        perror(argv[1]);
        exit(3);
    }

    // Vim 편집기를 실행하여 파일을 편집
    sprintf(command, "vim %s\n", argv[1]);
    system(command);

    // 파일의 잠금을 해제
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return 0;
}
