#include "header.h"
char *BASIC_PATH;
char *BACKUP_PATH;
dirNode *root;
char *ssl_mode;

void printRecoverUsage() {
    printf("Usage : recover <FILENAME> [OPTION]\n  -d : recover directory recursive\n  -n <NEWNAME> : recover file with new name\n");
}

//char *is_possiblePath() {} // 가능한 경로인지 파악

int ExceptRecover(int op_n, char **op, char *filename, int isExistD, int isExistN, char *newFilename) {
    char *tempPath = (char *)calloc(strlen(op[1])+7,sizeof(char));

    strcat(tempPath,BACKUP_PATH);
    strcat(tempPath,op[1]+strlen(BASIC_PATH));
    if(strlen(op[1]) > PATH_MAX || (isExistN && strlen(newFilename) > PATH_MAX)) { // 경로의 길이가 4096을 넘어갈 때
        printf("PilePath is too long\n");
        return -1;
    }
    if(get_filetype(tempPath)==0 && !isExistD) { // 디렉토리인데 -d 옵션을 주지 않은 경우
		printf("\"%s\" is a directory file\n",op[1]);
		return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    BASIC_PATH = getenv("HOME");
	BACKUP_PATH = (char *)calloc(strlen(BASIC_PATH)+8, sizeof(char));
	strcpy(BACKUP_PATH, BASIC_PATH);
	strcat(BACKUP_PATH,"/backup");
    if(argc<3 || argc>6) {
        printRecoverUsage();
        return 0;
    }
    char *absolutePath = makeHypoPath(argv[1],BASIC_PATH,BACKUP_PATH);
    if(absolutePath == NULL) {
        printRecoverUsage();
        return 0;
    }
    if(strncmp(absolutePath, BASIC_PATH, strlen(BASIC_PATH))!=0 \
    || strncmp(absolutePath, BACKUP_PATH, strlen(BACKUP_PATH))==0) { // Home 디렉토리를 벗어나거나 백업 디렉토리를 포함할 때
        printf("\"%s\" can't be backuped\n",argv[1]);
        return -1;
    }
    argv[1]=absolutePath;
    ssl_mode = (char *)calloc(strlen(argv[argc-1])+1,sizeof(char));
    strcat(ssl_mode,argv[argc-1]);
    argv[argc-1] = NULL;
    char **pathArr;
    int c;
    int path_n;
    char *newFilename; // 변경 할 파일명
    char **backupFileList; // 경로 안에 백업 할 파일들
    int isExistD=0, isExistN=0;
    int choose_n;

    root = (dirNode *)calloc(1,sizeof(dirNode));
	root->dirName = BASIC_PATH;
    makeFileTree(root,BASIC_PATH,BACKUP_PATH);
    pathArr = splitByCh(argv[1]+1,"/");
    path_n = ret2ArrSize(pathArr);
    while((c = getopt(argc-2, argv+1, "dn:")) != -1) { // 인자 처리
        // -1 means getopt() parse all options
        switch(c) {
            case 'd':
                isExistD = 1;
                break;
            case 'n':
                newFilename = optarg; // -n 뒤에 인자는 새로운 경로
                isExistN = 1;
                break;
            case '?':
                printRecoverUsage();
                return 0;
        }
    }
    if(isExistN) {
        newFilename = makeHypoPath(newFilename,BASIC_PATH,BACKUP_PATH);
        if(newFilename==NULL) {
            printRecoverUsage();
            return 0;
        }
        if(strncmp(newFilename, BASIC_PATH, strlen(BASIC_PATH))!=0 \
        || strncmp(newFilename, BACKUP_PATH, strlen(BACKUP_PATH))==0) { // Home 디렉토리를 벗어나거나 백업 디렉토리를 포함할 때
            printf("\"%s\" can't be backuped\n",argv[1]);
            return -1;
        }
    }
    if(ExceptRecover(argc,argv,pathArr[path_n-1],isExistD,isExistN,newFilename)==-1) return 0; // 예외 처리
    if(get_filetype(convertBackupPath(argv[1],BASIC_PATH,BACKUP_PATH))==0) { // 들어온 인자가 디렉토리인가
        root = findNode(root,argv[1]);
        char *oldPath = (char *)calloc(strlen(root->path)+strlen(root->dirName)+2,sizeof(char));
        strcat(oldPath,root->path);
        strcat(oldPath,"/");
        strcat(oldPath,root->dirName);
        if(isExistN) // 새로운 경로가 있다면
            backupFilesByLinked(root,BASIC_PATH,BACKUP_PATH,ssl_mode,oldPath,newFilename);
        else
            backupFilesByLinked(root,BASIC_PATH,BACKUP_PATH,ssl_mode,oldPath,NULL);
    }
    else { // 들어온 인자가 파일이면
        backupFileList = findFiles(argv[1], pathArr[path_n-1],BASIC_PATH,BACKUP_PATH);
        if(backupFileList == NULL) { // 백업 디렉토리에 해당 파일이 존재하지 않음
            printRecoverUsage();
            return -1;
        }
        if(ret2ArrSize(backupFileList)==1) { // 해당 파일이 하나일 시
            if(isExistN) // 새로운 경로가 있다면
                backupFile(backupFileList[0],BASIC_PATH,BACKUP_PATH,ssl_mode,newFilename);
            else
                backupFile(backupFileList[0],BASIC_PATH,BACKUP_PATH,ssl_mode,NULL);
        }
        else { // 해당 파일이 여러개 일때
            printf("backup file list of \"%s\"\n0. exit\n",argv[1]);
            printChooseList(backupFileList,1);
            printf(">> ");
            scanf("%d",&choose_n);
            if(choose_n==0) exit(0);
            if(isExistN)
                backupFile(backupFileList[choose_n-1],BASIC_PATH,BACKUP_PATH,ssl_mode,newFilename);
            else
                backupFile(backupFileList[choose_n-1],BASIC_PATH,BACKUP_PATH,ssl_mode,NULL);
        }
    }
}
