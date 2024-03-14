#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#define DIRECTORY_SIZE MAXNAMLEN


int main(int argc, char *argv[]){
	struct dirent *dentry;
	struct stat statbuf;
	char filename[DIRECTORY_SIZE+1];
	DIR *dirp;

	if(argc<2){
		fprintf(stderr,"usage: %s <directory>\n",argv[0]);
		exit(1);
	}

	if((dirp=opendir(argv[1]))==NULL||chdir(argv[1])==-1){ // opendir : 디렉토리 오픈, chdir : 작업 중인 디렉토리 변경
		fprintf(stderr, "opendir, chdir error for %s\n",argv[1]);
		exit(1);
	}

	while((dentry =readdir(dirp))!=NULL){ // 디렉토리 하위 파일 이름을 얻기 위해 리드
		if(dentry->d_ino==0)
			continue;

		memcpy(filename, dentry->d_name,DIRECTORY_SIZE); // 파일 이름 저장

		if(stat(filename,&statbuf)==-1){ // 파일을 스탯 구조체에 저장
			fprintf(stderr,"stat error for %s\n",filename);
			break;
		}

		if((statbuf.st_mode&S_IFMT)==S_IFREG)
			printf("%-14s %ld\n",filename,statbuf.st_size); // 파일명과 파일 크기 출력
		else
			printf("%-14s\n",filename);
	}
	exit(0);
}