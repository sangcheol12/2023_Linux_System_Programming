#include "header.h"

void appendNode(dirNode *parent, char *dName, char *originPath) { // 새로운 노드를 parent 밑에 추가
    for(int i=0; i<parent->child_n; i++)
        if(strcmp(parent->childNode[i]->dirName,dName)==0) // 이미 존재하는 노드일 때
            return ;
    dirNode *newNode = (dirNode *)calloc(1,sizeof(dirNode));
    newNode->dirName = (char *)calloc(strlen(dName)+1,sizeof(char));
    strcat(newNode->dirName,dName);
    newNode->path = (char *)calloc(strlen(originPath)+1,sizeof(char));
    strcat(newNode->path,originPath);
    parent->childNode = (dirNode **)realloc(parent->childNode,((parent->child_n)+1)*sizeof(dirNode *));
    parent->childNode[(parent->child_n)] = newNode;
    (parent->child_n)++;
}

void makeFileTree(dirNode *parent, char *originPath, char *backupPath) { // 프로그램 시작 시 백업 디렉토리를 트리구조로 생성 (재귀 사용)
    int child_n;
    struct dirent **namelist;
    int fileType;
    char *tempPath;
    char *tempBackupPath;
    int child_i = 0; // 자식 인덱스

    child_n = scandir(backupPath, &namelist, (void *)0, (void *)0);
    for(int i=0; i<child_n; i++) {
        if(strcmp(".",namelist[i]->d_name) == 0 || strcmp("..",namelist[i]->d_name) == 0) continue;
        tempPath = (char *)calloc(strlen(originPath)+strlen(namelist[i]->d_name) + 5,sizeof(char));
        tempBackupPath = (char *)calloc(strlen(backupPath)+strlen(namelist[i]->d_name) + 5,sizeof(char));
        strcat(tempPath,originPath);
        strcat(tempBackupPath,backupPath);
        strcat(tempBackupPath,"/");
        strcat(tempBackupPath,namelist[i]->d_name);
        fileType = get_filetype(tempBackupPath);
        if(fileType == 0) { // 디렉토리라면
            appendNode(parent,namelist[i]->d_name,originPath); // 노드에 디렉토리 추가 후
            strcat(tempPath,"/");
            strcat(tempPath,namelist[i]->d_name);
            makeFileTree(parent->childNode[child_i],tempPath,tempBackupPath); // 해당 디렉토리 재귀
            child_i++;
        } else if(fileType == 1) { // 파일이라면
            appendNode(parent,namelist[i]->d_name,originPath); // 노드에 파일 추가
            child_i++;
        }
        free(tempPath);
        free(tempBackupPath);
    }
}

void removeFilesByLinked(dirNode *parent, char *BASIC_PATH, char *BACKUP_PATH) { // 디렉토리 하위 파일을 모두 삭제 (재귀 사용)
    char *removePath;
    char *tempPath;

    for(int i=0; i<parent->child_n; i++) {
        tempPath = (char *)calloc(strlen(parent->childNode[i]->path)+strlen(parent->childNode[i]->dirName)+3,sizeof(char));
        strcat(tempPath,parent->childNode[i]->path);
        strcat(tempPath,"/");
        strcat(tempPath,parent->childNode[i]->dirName);
        removePath = convertBackupPath(tempPath,BASIC_PATH,BACKUP_PATH);
        if(get_filetype(removePath)==0) { // 디렉토리라면 하위 탐색
            removeFilesByLinked(parent->childNode[i],BASIC_PATH,BACKUP_PATH); // 하위 디렉토리로 이동하여 재귀적으로 삭제
            rmdir(removePath);
        } else {
            printf("\"%s\" backup file removed\n",removePath);
            remove(removePath); // 파일 삭제
        }
        free(tempPath);
    }
}

void clearLink(dirNode *parent, char *BASIC_PATH, char *BACKUP_PATH, int *dir_n, int *file_n) { // 재귀를 사용해 루트 및 모든 디렉토리 및 파일 삭제, 포인터로 삭제한 디렉토리 및 파일 카운트
    char *removePath;
    char *tempPath;

    for(int i=0; i<parent->child_n; i++) {
        tempPath = (char *)calloc(strlen(parent->childNode[i]->path)+strlen(parent->childNode[i]->dirName)+3,sizeof(char));
        strcat(tempPath,parent->childNode[i]->path);
        strcat(tempPath,"/");
        strcat(tempPath,parent->childNode[i]->dirName);
        removePath = convertBackupPath(tempPath,BASIC_PATH,BACKUP_PATH);
        if(get_filetype(removePath)==0) { // 디렉토리라면 하위 탐색
            clearLink(parent->childNode[i],BASIC_PATH,BACKUP_PATH, dir_n, file_n); // 재귀를 통해 하위 디렉토리 삭제
            (*dir_n)++;
            rmdir(removePath); // 디렉토리 삭제
        } else {
            remove(removePath); // 파일 삭제
            (*file_n)++;
        }
        free(tempPath);
    }
}

void backupFilesByLinked(dirNode *parent, char *BASIC_PATH, char *BACKUP_PATH, char *ssl_mode, char *oldPath, char *newPath) { // 재귀를 사용해 파일 백업
    int *alreadybackUp; // 이미 백업된 파일
    int already_n=0;
    char **same_file; // 같은 이름의 파일
    int same_n;
    int is_exist;
    int choose_n;

    alreadybackUp = (int *)calloc(1,sizeof(int));
    for(int i=0; i<parent->child_n; i++) {
        same_n = 0;
        is_exist = 0;
        for(int j=0; j<already_n; j++)
            if(i == alreadybackUp[j]) // 이미 백업한 파일일 경우
                is_exist=1;
        if(is_exist) continue;
        same_file = (char **)calloc(1,sizeof(char *));
        same_file[0] = (char *)calloc(strlen(parent->childNode[i]->path)+strlen(parent->childNode[i]->dirName)+3,sizeof(char));
        strcat(same_file[same_n],parent->childNode[i]->path);
        strcat(same_file[same_n],"/");
        strcat(same_file[same_n],parent->childNode[i]->dirName);
        if(get_filetype(convertBackupPath(same_file[same_n],BASIC_PATH,BACKUP_PATH))==0) { // 디렉토리라면 하위 디렉토리 탐색
            backupFilesByLinked(parent->childNode[i],BASIC_PATH,BACKUP_PATH,ssl_mode,oldPath,newPath); // 재귀적으로 하위 디렉토리 백업
            continue;
        }
        same_n++;
        alreadybackUp = (int *)realloc(alreadybackUp,(already_n+1)*sizeof(int));
        alreadybackUp[already_n++] = i;
        for(int j=i+1; j<parent->child_n; j++) {
            if(strlen(parent->childNode[i]->dirName)==strlen(parent->childNode[j]->dirName) \
            && strncmp(parent->childNode[i]->dirName,parent->childNode[j]->dirName,strlen((parent->childNode[i]->dirName))-13)==0) { // 이름의 길이와 백업시간을 제외한 이름이 같은지 검사
                alreadybackUp = (int *)realloc(alreadybackUp,(already_n+1)*sizeof(int));
                alreadybackUp[already_n] = j;
                same_file = (char **)realloc(same_file,(same_n+1)*sizeof(char *));
                same_file[same_n] = (char *)calloc(strlen(parent->childNode[j]->path)+strlen(parent->childNode[j]->dirName)+3,sizeof(char)); // same_list에 추가
                strcat(same_file[same_n],parent->childNode[j]->path);
                strcat(same_file[same_n],"/");
                strcat(same_file[same_n],parent->childNode[j]->dirName);
                same_n++;
            }
        }
        char *tempFile;
        char *newBackupPath;
        if(same_n==1) { // 같은 이름의 파일이 하나라면
            if(newPath==NULL) // 새 경로가 없다면
                backupFile(convertBackupPath(same_file[0],BASIC_PATH,BACKUP_PATH),BASIC_PATH,BACKUP_PATH,ssl_mode,NULL);
            else {
                newBackupPath = (char *)calloc(strlen(newPath)+strlen(same_file[0]),sizeof(char));
                strcat(newBackupPath,newPath);
                strcat(newBackupPath,same_file[0]+strlen(oldPath));
                backupFile(convertBackupPath(same_file[0],BASIC_PATH,BACKUP_PATH),BASIC_PATH,BACKUP_PATH,ssl_mode,newBackupPath);
            }
        }
        else { // 같은 이름이 여러개라면 선택 후 백업
            //same_file = (char **)realloc(same_file,(same_n+1)*sizeof(char *));
            tempFile = (char *)calloc(strlen(same_file[0]),sizeof(char));
            strncat(tempFile,same_file[0],strlen(same_file[0])-13);
            printf("backup file list of \"%s\"\n0. exit\n",tempFile);
            for(int t=0; t<same_n; t++)
                same_file[t] = convertBackupPath(same_file[t],BASIC_PATH,BACKUP_PATH);
            printChooseList(same_file,1);
            scanf("%d",&choose_n);
            if(choose_n==0) exit(0);
            if(newPath==NULL)
                backupFile(convertBackupPath(same_file[choose_n-1],BASIC_PATH,BACKUP_PATH),BASIC_PATH,BACKUP_PATH,ssl_mode,NULL);
            else {
                newBackupPath = (char *)calloc(strlen(newPath)+strlen(same_file[choose_n-1]),sizeof(char));
                strcat(newBackupPath,newPath);
                strcat(newBackupPath,same_file[0]+strlen(oldPath));
                backupFile(convertBackupPath(same_file[choose_n-1],BASIC_PATH,BACKUP_PATH),BASIC_PATH,BACKUP_PATH,ssl_mode,newBackupPath);
            }
            backupFile(same_file[choose_n-1],BASIC_PATH,BACKUP_PATH,ssl_mode,NULL);
        }
        free(same_file);
    }
}

dirNode *findNode(dirNode *root, char *path) { // 트리에서 들어온 경로에 해당하는 노드를 찾아주는 함수
    dirNode *cp;
    char **pathArr;
    int path_n;
    int depth = 2;
    cp = root;

    pathArr = splitByCh(path, "/");
    path_n = ret2ArrSize(pathArr);
    while(pathArr[depth]) {
        for(int i=0; i<cp->child_n; i++) {
            if(strcmp(pathArr[depth],cp->childNode[i]->dirName)==0) {
                cp = cp->childNode[i];
                break;
            }
        }
        depth++;
    }
    return cp;
}
