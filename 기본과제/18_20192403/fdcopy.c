#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define MSG "message will be written to Terminal\n"

int main(void)
{
    int new_fd;

    // STDOUT_FILENO(표준 출력 파일 디스크립터)를 복제하여 새로운 파일 디스크립터(new_fd)를 생성
    if ((new_fd = fcntl(STDOUT_FILENO, F_DUPFD, 3)) == -1) {
        fprintf(stderr, "Error: Copying File Descriptor\n");
        exit(1);
    }

    // 표준 출력 파일 디스크립터를 닫음
    close(STDOUT_FILENO);

    // 새로운 파일 디스크립터를 사용하여 메시지를 출력
    write(3, MSG, strlen(MSG));

    exit(0);
}
