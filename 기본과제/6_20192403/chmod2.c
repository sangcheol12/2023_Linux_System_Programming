#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define MODE_EXEC (S_IXUSR|S_IXGRP|S_IXOTH) // 소유자, 그룹 사용자와 다른 사용자의 실행 권한 지정

int main(int argc, char* argv[]){
	struct stat statbuf;
	int i;
	if(argc<2){
		fprintf(stderr,"usage: %s <file1> <file2> ... <fileN>\n",argv[0]);
		exit(1);
	}
	for(i=1;i<argc;i++){
		if(stat(argv[i],&statbuf)<0){
			fprintf(stderr,"%s : stat error\n",argv[i]);
			continue;
		}

		statbuf.st_mode |= MODE_EXEC;
		statbuf.st_mode ^= (S_IXGRP|S_IXOTH); // 그룹 사용자와 다른 사용자의 실행 권한 지정

		if(chmod(argv[i],statbuf.st_mode)<0)
			fprintf(stderr,"%s : chmod error\n",argv[i]);
		else
			printf("%s : file permission was changed.\n",argv[i]);
	}
	exit(0);
}