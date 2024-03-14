#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#define PATH_MAX 4096

typedef struct dirNode {
    char *dirName;
    char *path;
    int child_n;
    struct dirNode **childNode;
} dirNode;

//common
int ret2ArrSize(char **arr);
int ret2ArrLen(char **arr);
char **splitByCh(char *str, char *ch);
int get_filetype(char *path);
char *retCurTime();
char **findFiles(char *path, char *filename, char *BASIC_PATH, char *BACKUP_PATH);
void printChooseList(char **fileList, int mode);
char *convertBackupPath(char *originPath, char *BASIC_PATH, char *BACKUP_PATH);
char *makeHypoPath(char *str, char *BASIC_PATH, char *BACKUP_PATH);

//hash
char *do_fp_md(FILE *f);
char *do_fp_sha(FILE *f);
int isSameFile(char *originPath, char *dupPath, char *mode);

//linkedList
void appendNode(dirNode *parent, char *dName, char *originPath);
void makeFileTree(dirNode *parent, char *originPath, char *backupPath);
void removeFilesByLinked(dirNode *parent, char *BASIC_PATH, char *BACKUP_PATH);
void clearLink(dirNode *parent, char *BASIC_PATH, char *BACKUP_PATH, int *dir_n, int *file_n);
void backupFilesByLinked(dirNode *parent, char *BASIC_PATH, char *BACKUP_PATH, char *ssl_mode, char *oldPath, char *newPath);
dirNode *findNode(dirNode *root, char *path);

int backupFile(char *backupPath, char *BASIC_PATH, char *BACKUP_PATH, char *ssl_mode, char *newPath);