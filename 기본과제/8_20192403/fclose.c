#include <stdio.h>
#include <stdlib.h>

/*i
  파일을 오픈 했다가 닫는 함수
*/
int main(void){
	char *fname="ssu_test.txt";
	FILE *fp;

	if((fp=fopen(fname,"r"))==NULL){ // 파일을 읽기 전용으로 오픈
		fprintf(stderr,"fopen error for %s\n",fname);
		exit(1);
	}
	else{
		printf("Success!\n");
		printf("Opening\"%s\" in \"r\" mode!\n",fname);
	}

	fclose(fp); // 오픈한 파일 클로즈
	exit(0);
}
