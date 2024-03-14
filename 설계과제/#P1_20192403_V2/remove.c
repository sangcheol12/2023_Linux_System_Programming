#include "header.h"
char *BASIC_PATH;
char *BACKUP_PATH;
dirNode *root;

void printRemoveUsage() {
    printf("Usage : remove <FILENAME> [OPTION]\n  -a : remove all file(recursive)\n  -c : clear backup directory\n");
}

int ExceptRemove(int op_n, char **op, char *filename, int isExistA) {
    char *tempPath = (char *)calloc(strlen(op[1])+8,sizeof(char));

    strcat(tempPath,BACKUP_PATH);
    strcat(tempPath,op[1]+strlen(BASIC_PATH));
    if(strlen(op[1]) > PATH_MAX) { // 경로의 길이가 4096을 넘어갈 때
        printf("PilePath is too long\n");
        return -1;
    }
    if(get_filetype(tempPath)==0 && !isExistA) { // 디렉토리인데 -a 옵션을 주지 않은 경우
		printf("\"%s\" is a directory file\n",op[1]);
		return -1;
    }
    free(tempPath);
    return 0;
}

int main(int argc, char **argv) {
    BASIC_PATH = getenv("HOME");
	BACKUP_PATH = (char *)calloc(strlen(BASIC_PATH)+8, sizeof(char));
	strcpy(BACKUP_PATH, BASIC_PATH);
	strcat(BACKUP_PATH,"/backup");
    root = (dirNode *)calloc(1,sizeof(dirNode));
	root->dirName = BASIC_PATH;
    makeFileTree(root,BASIC_PATH,BACKUP_PATH);
    if(argc==1) {
        printRemoveUsage();
        return 0;
    }
    if(argc==2) { // 인자가 두개일 때
        int dir_n=0,file_n=0;
        if(strcmp(argv[1],"-c")==0) { // 두번 째 인자는 무조건 -c임
            clearLink(root,BASIC_PATH,BACKUP_PATH,&dir_n,&file_n); // 모든 백업 파일내의 디렉토리와 파일 삭제
            if(dir_n==0 && file_n==0) {
                printf("no file(s) in the backup\n");
            }
            printf("backup directory cleared(%d regular files and %d subdirectories totally).\n",file_n,dir_n);
            return 0;
        }
    }
    int c;
    int isExistA=0;
    while((c = getopt(argc-1, argv+1, "ac")) != -1) {
        // -1 means getopt() parse all options
        switch(c) {
            case 'a':
                isExistA = 1;
                break;
            case 'c':
                printRemoveUsage();
                return 0;
            case '?':
                printRemoveUsage();
                return 0;
        }
    }
    char *absolutePath = makeHypoPath(argv[1],BASIC_PATH,BACKUP_PATH);
    if(absolutePath == NULL) {
        printRemoveUsage();
        return 0;
    }
    if(strncmp(absolutePath, BASIC_PATH, strlen(BASIC_PATH))!=0 || strncmp(absolutePath, BACKUP_PATH, strlen(BACKUP_PATH))==0) { // 경로를 벗어난 예외
        printf("\"%s\" can't be backuped\n",argv[1]);
        return -1;
    }
    argv[1] = absolutePath;

    char **pathArr;
    int path_n;
    char **removeFileList;
    int choose_n;

    pathArr = splitByCh(argv[1]+1,"/");
    path_n = ret2ArrSize(pathArr);
    if(ExceptRemove(argc,argv,pathArr[path_n-1],isExistA)==-1) return 0; // 예외처리
    if(get_filetype(convertBackupPath(argv[1],BASIC_PATH,BACKUP_PATH))==0) { // 들어온 인자가 디렉토리
        root = findNode(root,argv[1]);
        removeFilesByLinked(root,BASIC_PATH,BACKUP_PATH); // 링크드 리스트로 폴더 삭제
    }
    else { // 인자가 파일
        removeFileList = findFiles(argv[1], pathArr[path_n-1],BASIC_PATH,BACKUP_PATH);
        if(removeFileList == NULL) { // 백업 디렉토리에 해당 파일이 존재하지 않음
            printRemoveUsage();
            return -1;
        }
        if(ret2ArrSize(removeFileList)==1) {
            printf("\"%s\" backup file removed\n",removeFileList[0]);
            remove(removeFileList[0]);
        }
        else {
            if(isExistA) { // -a가 있을때 모두 삭제
                int i=0;
                while (removeFileList[i])
                {
                    printf("\"%s\" backup file removed\n",removeFileList[i]);
                    remove(removeFileList[i]);
                    i++;
                }
            } else { // 하나만 입력 받아 삭제
                printf("backup file list of \"%s\"\n0. exit\n",argv[1]);
                printChooseList(removeFileList,0);
                scanf("%d",&choose_n);
                if(choose_n==0) exit(0);
                printf("\"%s\" backup file removed\n",removeFileList[choose_n-1]);
                remove(removeFileList[choose_n-1]);
            }
        }
    }
}
