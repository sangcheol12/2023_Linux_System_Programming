#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(void){
	char *fname="ssu_test.dat";
	char name[BUFFER_SIZE];
	FILE *fp;
	int age;

	fp=fopen(fname,"r");
	fscanf(fp,"%s%d", name,&age); // 파일에서 이름과 나이를 읽어옴
	fclose(fp);
	fp=fopen(fname,"w");
	fprintf(fp,"%s is %d years old\n", name, age); // 읽어 온 값을 바탕으로 형식대로 파일에 출력
	fclose(fp);
	exit(0);
}