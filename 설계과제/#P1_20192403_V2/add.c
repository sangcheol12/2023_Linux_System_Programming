#include "header.h"
char *BASIC_PATH;
char *BACKUP_PATH;
char *ssl_mode;
dirNode *root;

void printAddUsage() {
    printf("Usage : add <FILENAME> [OPTION]\n -d : add directory recursive\n");
}

int ExceptAdd(char **op, int op_n) {
	if(strncmp(BASIC_PATH,op[1],strlen(BASIC_PATH)) != 0 \
	|| strncmp(BACKUP_PATH,op[1],strlen(BACKUP_PATH)) == 0) { // 홈 디렉터리 벗어나거나 백업 디렉토리 포함 예외
		printf("\"%s\" can't be backuped\n",op[1]);
		return -1;
	}
    if(access(op[1], F_OK) < 0) { // 존재하지 않는 디렉토리
        printAddUsage();
        return -1;
    }
    if(strlen(op[1]) > PATH_MAX) {
        printf("PilePath is too long\n"); // 절대 경로가 4096을 넘는 경우
        return -1;
    }

    if(access(op[1], F_OK) < 0) { // 존재하지 않는 디렉토리
        printAddUsage();
        return -1;
    }

	if(get_filetype(op[1])==0 && op_n<3) { // 디렉토리인데 -d 옵션을 주지 않은 경우
		printf("\"%s\" is a directory file\n",op[1]);
		return -1;
	}

	if(get_filetype(op[1])!=0 && get_filetype(op[1])!=1) { // 일반 파일이나 디렉토리가 아닌 경우
		printf("\"%s\" is not general file or directory\n",op[1]);
		return -1;
	}
	if(op_n==3 && strcmp(op[2],"-d")!=0) { // 인자가 올바르지 않은 경우
		printAddUsage();
		return -1;
	}
	if(access(op[1],R_OK) < 0) { // 파일의 접근 권환이 없는 경우
		printf("\"%s\" can't access\n",op[1]);
		return -1;
	}
	return 0;
}

int dupBackupFile(char *path, char *backupPath, char *filename) { // 지정 경로애 파일을 복사하는 함수
    FILE *originFd,*dupFd;
    struct dirent **namelist;
    int child_n;
    char *tempFilePath;
    char c;

    tempFilePath = (char *)calloc(strlen(backupPath)+strlen(filename)+15,sizeof(char));
    strcat(tempFilePath,backupPath);
    strcat(tempFilePath,"/");
    child_n = scandir(backupPath,&namelist,(void *)0,(void *)0);
    for(int i=0; i<child_n; i++) {
        if(strcmp(namelist[i]->d_name,".") == 0 || strcmp(namelist[i]->d_name,"..") == 0)
            continue;
        if(strncmp(namelist[i]->d_name,filename,strlen(namelist[i]->d_name)-13) == 0) { // 백업 시간을 제외하고 파일의 이름이 같은지 검사
            strcat(tempFilePath,namelist[i]->d_name);
            if(isSameFile(path,tempFilePath,ssl_mode) == 1) { // 파일의 해쉬값이 같은지 검사
                printf("\"%s\" is already backuped\n",tempFilePath);
                return 0;
            }
            memset(tempFilePath+strlen(backupPath)+1,0,strlen(namelist[i]->d_name)+1);
        }
    }
    strcat(tempFilePath,filename);
    strcat(tempFilePath,"_");
    strcat(tempFilePath,retCurTime()); // 백업 파일명 뒤에 백업 시간 추가
    if((originFd = fopen(path,"r+")) < 0) // 원본 파일 오픈
        return -1;
    if((dupFd = fopen(tempFilePath,"w+")) < 0) { // 백업 될 파일 생성
        fclose(originFd);
        return -1;
    }
    while(1) {
        c = fgetc(originFd);
        if(!feof(originFd))
            fputc(c, dupFd);
        else
            break;
    }
    printf("\"%s\" backuped\n",tempFilePath);
    //appendNode();
    fclose(originFd);
    fclose(dupFd);
    return 0;
}

void recursiveAddDir(char *originPath, char *backupPath) { // 디렉토리를 재귀적으로 백업을 진행하는 함수
    struct dirent **namelist;
    int child_n;
    int fileType;
    char *tempPath;
    char *tempBackupPath;
    dirNode *header;

    mkdir(backupPath,0755); // 백업 디렉토리에 해당 디렉토리가 없다면 생성
    //if(mkdir(backupPath,0755) >= 0)
        //appendNode();
    child_n = scandir(originPath, &namelist, (void *)0, (void *)0);
    for(int i=0; i<child_n; i++) {
        if(strcmp(".",namelist[i]->d_name) == 0 || strcmp("..",namelist[i]->d_name) == 0) continue;
        tempPath = (char *)calloc(strlen(originPath)+strlen(namelist[i]->d_name) + 5,sizeof(char)); // 원본 경로
        tempBackupPath = (char *)calloc(strlen(backupPath)+strlen(namelist[i]->d_name) + 5,sizeof(char)); // 백업 디렉토리 경로
        strcat(tempPath,originPath);
        strcat(tempBackupPath,backupPath);
        strcat(tempPath,"/");
        strcat(tempPath,namelist[i]->d_name);
        fileType = get_filetype(tempPath);
        if(fileType == 0) { // 만약 디렉토리라면
            strcat(tempBackupPath,"/");
            strcat(tempBackupPath,namelist[i]->d_name);
            recursiveAddDir(tempPath,tempBackupPath); // 재귀 호출을 통해 하위 디렉토리에서 백업 다시 진행
        } else if(fileType == 1) // 파일이라면
            dupBackupFile(tempPath, tempBackupPath, namelist[i]->d_name); // 해당 파일 백업
        free(tempPath);
        free(tempBackupPath);
    }
}

int addPathDir(char **pathArr) {
    char *path;
    char *backupPath;
    int path_n;
    int depth=2;
    int fileType;

    path_n = ret2ArrSize(pathArr);
    path = (char *)calloc(ret2ArrLen(pathArr)+path_n+5,sizeof(char));
    backupPath = (char *)calloc(ret2ArrLen(pathArr)+path_n+10,sizeof(char));
    strcat(path,BASIC_PATH);
    strcat(backupPath,BACKUP_PATH);
    while(depth < path_n) {
        strcat(path,"/");
        strcat(path,pathArr[depth]);
        if(depth != path_n-1) { // 마지막 경로를 제외하고 백업 경로 추가 (백업 경로와 파일명을 분리하여 파일 생성을 더 용이하게 하기 위함)
            strcat(backupPath,"/");
            strcat(backupPath,pathArr[depth]);
        }
        mkdir(backupPath,0755); // 파일을 추가하기 위해 백업 디렉토리에 하위 디렉토리 경로가 없다면 추가
        //if(mkdir(backupPath,0755) >= 0)
            //appendNode();
        depth++;
    }
    fileType = get_filetype(path);
    if(fileType == 0) { // 디렉토리라면
        strcat(backupPath,"/");
        strcat(backupPath,pathArr[depth-1]);
        recursiveAddDir(path, backupPath); // 재귀로 하위까지 모두 백업
    } else if(fileType == 1)  { // 파일이라면
        dupBackupFile(path,backupPath,pathArr[depth-1]); // 파일 백업
    }
    return 0;
}

int main(int argc, char **argv) {
    BASIC_PATH = getenv("HOME");
	BACKUP_PATH = (char *)calloc(strlen(BASIC_PATH)+8, sizeof(char));
	strcpy(BACKUP_PATH, BASIC_PATH);
	strcat(BACKUP_PATH,"/backup");
    int i=0;
    char *absolutePath;

    if (argc == 2 || argc>4 || argv[1]==NULL) { // 인자가 하나인 경우 이거나 3개 이상일 때 (ssl_mode로 인해 +1 됨)
        printAddUsage();
        return 0;
    }
    absolutePath = realpath(argv[1], absolutePath); // 들어온 경로가 상대경로이면 절대경로로 변환
    if(absolutePath==NULL) {
        printf("\"%s\" can't be backuped\n",argv[1]);
        return -1;
    }
    if(strncmp(BASIC_PATH,absolutePath,strlen(BASIC_PATH)) != 0 \
	|| strncmp(BACKUP_PATH,absolutePath,strlen(BACKUP_PATH)) == 0) { // 홈 디렉터리 벗어나거나 백업 디렉토리 포함 예외
		printf("\"%s\" can't be backuped\n",argv[1]);
		return -1;
	}
	argv[1]=absolutePath;
    ssl_mode = (char *)calloc(strlen(argv[argc-1])+1,sizeof(char)); // 해쉬 모드(md5,sha1) 추출
    strcat(ssl_mode,argv[argc-1]);
    argv[argc-1] = NULL;
    char **pathArr; // 절대경로 디렉토리,파일 단위로 분리
    int path_n;

    root = (dirNode *)calloc(1,sizeof(dirNode));
	root->dirName = BASIC_PATH;
    makeFileTree(root,BASIC_PATH,BACKUP_PATH); // 링크드리스트 생성
    pathArr = splitByCh(argv[1]+1,"/");
    path_n = ret2ArrSize(pathArr);
    if(ExceptAdd(argv,argc-1)==-1) return 0; // 예외처리
    addPathDir(pathArr); // 파일 복제
}
