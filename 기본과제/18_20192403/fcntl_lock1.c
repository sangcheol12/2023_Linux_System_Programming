#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define NAMESIZE 50
#define MAXTRIES 5

struct employee {
    char name[NAMESIZE];
    int salary;
    int pid;
};

int main(int argc, char *argv[]) {
    struct flock lock;
    struct employee record;
    int fd, sum = 0, try = 0;

    // 10초 동안 대기
    sleep(10);

    // 명령줄 인수로 전달된 파일을 읽기 전용으로 오픈
    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror(argv[1]);
        exit(1);
    }

    // 파일에 읽기 잠금을 설정
    lock.l_type = F_RDLCK;
    lock.l_whence = 0;
    lock.l_start = 0L;
    lock.l_len = 0L;

    // 파일에 잠금을 설정
    while (fcntl(fd, F_SETLK, &lock) == -1) {
        if (errno == EACCES) {
            if (try++ < MAXTRIES) {
                sleep(1);
                continue;
            }
            printf("%s busy -- try later\n", argv[1]);
            exit(2);
        }
        perror(argv[1]);
        exit(3);
    }

    sum = 0;
    // 파일에서 레코드를 읽어서 출력하고 급여의 총합을 계산
    while (read(fd, (char *)&record, sizeof(record)) > 0) {
        printf("Employee: %s, Salary: %d\n", record.name, record.salary);
        sum += record.salary;
    }
    printf("\nTotal salary: %d\n", sum);

    // 파일의 잠금을 해제
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}
