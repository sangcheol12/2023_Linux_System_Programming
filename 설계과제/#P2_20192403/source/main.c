#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_score.h"

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);

int main(int argc, char *argv[])
{
	struct timeval begin_t, end_t; // 시간을 저장하기 위한 구조체(초, 마이크로초)
	gettimeofday(&begin_t, NULL); // 시작 시간 저장

	ssu_score(argc, argv);

	gettimeofday(&end_t, NULL); // 종료 시간 저장
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t->tv_sec -= begin_t->tv_sec; // 프로그램 동작 시간 = 시작 시간 - 종료시간

	if(end_t->tv_usec < begin_t->tv_usec){
		end_t->tv_sec--;
		end_t->tv_usec += SECOND_TO_MICRO;
	}

	end_t->tv_usec -= begin_t->tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec); // 동작 시간 출력
}
