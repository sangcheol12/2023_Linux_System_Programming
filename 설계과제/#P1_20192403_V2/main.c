#include "header.h"
char *BASIC_PATH;
char *BACKUP_PATH;

void printUsage() {
    printf("Usage: ssu_backup <md5 | sha1>\n");
}

char opTable[8][8] = {"add","remove","recover","ls","vi","vim","help","exit"};
int hashConvertOP(char *op) {
	int i;
	for(i=0; i<8; i++)
		if(!strcmp(opTable[i],op))
			break;
	return i;
}

int main(int argc, char **argv) {
    char inputData[5000]; // 입력 받는 문자열
	char **op; // 공백 단위로 나눈 명령어 집합
	int op_n; // 명령어의 수
	char *absolutePath; // 절대경로
    if(argc != 2) {
        printUsage();
        return 0;
    }
    if(strcmp(argv[1],"md5") != 0 && strcmp(argv[1],"sha1") != 0) {
        printUsage();
        return 0;
    }
	BASIC_PATH = getenv("HOME"); // 사용자의 홈 경로 가져옴
	BACKUP_PATH = (char *)calloc(strlen(BASIC_PATH)+8,sizeof(char)); // 사용자의 홈의 디렉토리 경로 가져옴
	strcpy(BACKUP_PATH, BASIC_PATH);
	strcat(BACKUP_PATH,"/backup");
	mkdir(BACKUP_PATH,0755);

	while (1)
	{
		printf("20192403> ");
    	fgets(inputData, sizeof(inputData), stdin); // 줄 단위 입력
		inputData[strlen(inputData)-1]=0; // 마지막 개행 문자 제거
		op = splitByCh(inputData, " ");
		op_n = ret2ArrSize(op);

		pid_t pid = fork();
		if(pid < 0) {
			printf("fork error\n");
		}
		else if(pid == 0) {
			switch (hashConvertOP(op[0]))
			{
				case 0: // add
					op = (char **)realloc(op,sizeof(char *)*(op_n+2));
					op[op_n] = argv[1];
					op[op_n+1] = NULL;
					execv("./add",op);
					break;
				case 1: // remove
					execv("./remove",op);
					break;
				case 2: // recover
					op = (char **)realloc(op,sizeof(char *)*(op_n+2));
					op[op_n] = argv[1];
					op[op_n+1] = NULL;
					execv("./recover",op);
					break;
				case 3: // ls
					execl("/bin/ls","ls",NULL);
					break;
				case 4: /// vi
					execl("/usr/bin/vi","vi",NULL);
					break;
				case 5: // vim
					execl("/usr/bin/vim","vim",NULL);
					break;
				case 6: // help
					execl("./help","help",NULL);
					break;
				case 7: // exit
					exit(0);
					break;
				default: // other op
					execl("./help","help",NULL);
					break;
			}
		}
		else {
			if(hashConvertOP(op[0])==7)
				exit(0);
			int wc = wait(NULL); // 자식의 프로세스가 종료되기를 기다림
		}
	}
}
