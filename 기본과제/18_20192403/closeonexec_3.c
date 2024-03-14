#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main(void)
{
    int flag;

    // STDOUT_FILENO의 CLOSE_ON_EXEC 플래그 값을 가져옴
    if ((flag = fcntl(STDOUT_FILENO, F_DUPFD)) == -1) {
        fprintf(stderr, "Error: Checking CLOSE_ON_EXEC\n");
        exit(1);
    }

    // CLOSE_ON_EXEC 플래그 값을 출력
    printf("CLOSE ON EXEC flag is = %d\n", flag);
    exit(0);
}
