#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main(void)
{
    int testfd;
    int fd;

    fd = open("test.txt", O_CREAT);

    // 파일 디스크립터 fd를 사용하여 파일을 열고, testfd에 새로운 파일 디스크립터를 할당
    testfd = fcntl(fd, F_DUPFD, 5);
    printf("testfd : %d\n", testfd);

    // 파일 디스크립터 fd를 사용하여 파일을 열고, 다시 testfd에 새로운 파일 디스크립터를 할당
    testfd = fcntl(fd, F_DUPFD, 5);
    printf("testfd : %d\n", testfd);

    getchar();
}
