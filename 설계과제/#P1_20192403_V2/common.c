#include "header.h"

int ret2ArrSize(char **arr) { // 2차원 배열의 크기 반환
    int i=0;
    while(arr[i])
        i++;
    return i;
}

int ret2ArrLen(char **arr) { // 2차원 배열의 전체 문자열 길이 반환
    int i=0;
    int cnt=0;
    while(arr[i]) {
        cnt += strlen(arr[i]);
        i++;
    }
    return cnt;
}

char **splitByCh(char *str, char *ch) { // 인자로 들어온 문자열을 인자로 들어온 문자로 분할하여 2차원 배열로 반환
    char **ret;
    int i;
    int size=1;
    char *new_str = (char *)malloc(strlen(str)+1);
    strcpy(new_str,str);
    for(i=0; i<strlen(new_str); i++)
        size++;
    ret = (char **)malloc(sizeof(char *) * (size+1));
    i=0;
    ret[i] = strtok(new_str, ch);    //첫번째 strtok 사용.
    while (ret[i] != NULL)              //ptr이 NULL일때까지 (= strtok 함수가 NULL을 반환할때까지)
    {
        i++;
        ret[i] = strtok(NULL, ch);     //자른 문자 다음부터 구분자 또 찾기
    }
    return ret;
}

int get_filetype(char *path) {
    struct stat statbuf;
    if(stat(path,&statbuf)<0)
        return -1;
    if(S_ISDIR(statbuf.st_mode)) // 디렉토리
        return 0;
    if(S_ISREG(statbuf.st_mode)) // 일반 파일
        return 1;
    if(S_ISCHR(statbuf.st_mode)) // 캐릭터 파일
        return 2;
    if(S_ISBLK(statbuf.st_mode)) // 블록 파일
        return 3;
    if(S_ISFIFO(statbuf.st_mode)) // 파이포 파일
        return 4;
    return -1;
}

char *retCurTime() {
    time_t timer;
    struct tm *t;
    char *date = (char *)calloc(20,sizeof(char));

    timer = time(NULL);
    t = localtime(&timer);
    strftime(date, 20, "%Y%m%d%H%M%S", t); // tm 구조체에서 날짜를 buf 로
    return date+2;
}

char **findFiles(char *path, char *filename, char *BASIC_PATH, char *BACKUP_PATH) {
    char **files=NULL; // 파일들의 경로를 저장할 2차원 포인터
    int n=0;
    struct dirent **namelist;
    int child_n;
    char *tempPath = (char *)calloc(strlen(path)-strlen(filename)+7,sizeof(char));

    strcat(tempPath,BACKUP_PATH);
    strncat(tempPath,path+strlen(BASIC_PATH),strlen(path)-strlen(BASIC_PATH)-strlen(filename)-1);
    child_n = scandir(tempPath,&namelist,(void *)0,(void *)0); // 하위 파일들 목록 가져오기
    for(int i=0; i<child_n; i++) {
        if(strcmp(namelist[i]->d_name,".")==0 || strcmp(namelist[i]->d_name,"..")==0) continue; // 현재 및 부모 디렉토리 예외
        if((strncmp(filename,namelist[i]->d_name,strlen(filename))==0) && \
        (strlen(namelist[i]->d_name)==strlen(filename)+13)) { // 백업 시간을 제외한 파일명이 같다면 files 포인터에 추가
            files = (char **)realloc(files, sizeof(char *)*(n+1));
            files[n] = (char *)calloc(strlen(path)+strlen(namelist[i]->d_name)+2,sizeof(char));
            strcat(files[n],tempPath);
            strcat(files[n],"/");
            strcat(files[n],namelist[i]->d_name);
            n++;
        }
    }
    if(n==0)
        return NULL;
    files = (char **)realloc(files, sizeof(char *)*(n));
    files[n] = NULL;
    return files;
}

void printChooseList(char **fileList, int mode) {
    struct stat statbuf;
    for(int i=1; i<=ret2ArrSize(fileList); i++) {
        if(stat(fileList[i-1],&statbuf)<0)
            return ;
        printf("%d. %s\t\t%lldbytes\n",i,fileList[i-1]+strlen(fileList[i-1])-12,statbuf.st_size); // 백업 파일 리스트로 들어온 파일의 백업 시간과 크기 출력
    }
    if(mode==0) printf("Choose file to remove\n>> ");
    if(mode==1) printf("Choose file to recover\n>> ");
}

char *convertBackupPath(char *originPath, char *BASIC_PATH, char *BACKUP_PATH)  { // 인자로 들어온 경로를 백업 디렉토리 경로로 변경
    char *tempPath = (char *)calloc(strlen(originPath)+9,sizeof(char));

    strcat(tempPath,BACKUP_PATH);
    strcat(tempPath,originPath+strlen(BASIC_PATH));
    return tempPath;
}

char *makeHypoPath(char *str, char *BASIC_PATH, char *BACKUP_PATH) { // 존재 유무를 확실히 모르는 상대경로로 가상의 절대경로 만드는 함수
	char **pathArr = splitByCh(str,"/");
    int path_n = ret2ArrSize(pathArr);
	char *tempPath,*retPath;
	tempPath = (char *)calloc(strlen(str)+5,sizeof(char));
	int i=0;
    retPath = (char *)calloc(1,sizeof(char));
	if(strcmp(pathArr[0],".")==0 || strcmp(pathArr[0],"..")==0) {
		while(i < path_n && (strcmp(pathArr[i],".")==0 || strcmp(pathArr[i],"..")==0)) {
			strcat(tempPath,pathArr[i]);
            strcat(tempPath,"/");
			i++;
		}
        retPath = realpath(tempPath,retPath);
	} else if(strcmp(pathArr[0],"~")==0) {
        retPath = realpath(getenv("HOME"),tempPath);
		i++;
    } else if(str[0] != '/') {
        retPath = realpath("./",tempPath);
    }
    tempPath = (char *)realloc(tempPath,(strlen(retPath)+1)*sizeof(char));
    memset(tempPath,0,(strlen(retPath)+1)*sizeof(char));
    strcat(tempPath,retPath);
	while(pathArr[i]) {
        if(i>=path_n-1) break; // 마지막 파일 전까지
		tempPath = (char *)realloc(tempPath,sizeof(char)*(strlen(tempPath)+strlen(pathArr[i])+2));
		strcat(tempPath,"/");
		strcat(tempPath,pathArr[i]);
		i++;
	}
    if(strncmp(BASIC_PATH,tempPath,strlen(BASIC_PATH)) != 0 \
	|| strncmp(BACKUP_PATH,tempPath,strlen(BACKUP_PATH)) == 0) { // 홈 디렉터리 벗어나거나 백업 디렉토리 포함 예외
		return tempPath;
	}
    char *tempBackupPath;
    tempBackupPath = convertBackupPath(tempPath,BASIC_PATH,BACKUP_PATH);
    tempPath = realpath(tempBackupPath,tempPath);
    if(tempPath == NULL) return NULL; // 마지막 파일 혹은 디렉토리 전까지 경로가 백업 디렉토리에 없다면 NULL 반환
    retPath = (char *)malloc(strlen(tempPath));
    strcat(retPath,BASIC_PATH);
    strcat(retPath,tempPath+strlen(BACKUP_PATH));
    if(i < path_n) {
        retPath = (char *)realloc(retPath,sizeof(char)*(strlen(retPath)+strlen(pathArr[path_n-1])+2));
		strcat(retPath,"/");
		strcat(retPath,pathArr[i]);
    }
	return retPath;
}

int backupFile(char *backupPath, char *BASIC_PATH, char *BACKUP_PATH, char *ssl_mode, char *newPath) { // 들어은 백업 파일의 경로에 백업 파일 생성
    char *originPath;
    FILE *originFd, *dupFd;
    char **pathArr;
    int path_n;
    char *tempPath;
    char c;

    if(newPath == NULL) { // 새로운 경로 NULL이라면 원래 경로로 설정
        originPath = (char *)calloc(strlen(backupPath)-19,sizeof(char));
        strcat(originPath,BASIC_PATH);
        strncat(originPath,backupPath+strlen(BACKUP_PATH),strlen(backupPath)-strlen(BACKUP_PATH)-13);
    } else // 새로운 경로가 있다면 경로 변경
        originPath = newPath;
    if(isSameFile(originPath,backupPath,ssl_mode)==1) {
        printf("\"%s\" is already backuped\n",backupPath);
        return -1;
    }
    pathArr = splitByCh(originPath,"/");
    path_n = ret2ArrSize(pathArr);
    tempPath = (char *)calloc(strlen(originPath)+1,sizeof(char));
    strcat(tempPath,BASIC_PATH);
    for(int i=2; i<path_n-1; i++) {
        strcat(tempPath,"/");
        strcat(tempPath,pathArr[i]);
        mkdir(tempPath,0755);
    }
    if((dupFd = fopen(backupPath,"r+")) == NULL) // 백업 할 파일 열기
        return -1;
    if((originFd = fopen(originPath,"w+")) == NULL) { // 백업 될 경로에 파일 생성
        fclose(originFd);
        return -1;
    }
    while(1) { // 파일의 끝을 만날때까지 한글자씩 읽어서 복사
        c = fgetc(dupFd);
        if(!feof(dupFd))
            fputc(c, originFd);
        else
            break;
    }
    printf("\"%s\" backup recover to \"%s\"\n",backupPath,originPath);
    return 0;
}
