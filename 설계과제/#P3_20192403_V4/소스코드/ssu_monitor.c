#include "ssu_monitor.h"

FILE *log_fp;
char *ID = "20192403";
char *monitor_list = "monitor_list.txt";
volatile sig_atomic_t signal_received = 0;
int execute_time;

void ssu_monitor(int argc, char *argv[]) {
	ssu_prompt();
	return;
}

void ssu_prompt(void) {
    int argc;
    char temp[5][FILELEN] = {0};
	char *argv[5];
    char command[BUFLEN] = "";
	while (1) {
        memset(command, 0, sizeof(command));
        for(int i=0; i<(sizeof(temp)/sizeof(temp[0])); i++)
            memset(temp[i], 0, sizeof(temp[i]));
		printf("%s> ", ID);
		fgets(command, sizeof(command), stdin); // 줄 단위 입력 받기
        command[strlen(command)-1] = 0; // 마지막 개행문자 제거
        // 공백 단위로 문자열 분리
        argc = splitByCh(temp, command, " ");
		for (int i = 0; i < argc; i++)
        	argv[i] = temp[i];
		argv[argc] = NULL;
        if (argc <= 0) {
            execute_help(argc, argv);
            continue;
        }
		if (execute_command(argc, argv) == 1) // exit()
            return ;
	}
}

int execute_command(int argc, char *argv[]) {
    if (!strcmp(argv[0], "add")) {
        // 'add' 명령어 실행
        if (argc != 2 && argc != 4) {
            execute_help(argc, argv);
            return 0;
        }
        execute_time = 1;
        if (argc == 4 && !check_add_option(argc, argv)) {
            execute_help(argc, argv);
            return 0;
        }
        execute_add(argc, argv);
    } else if (!strcmp(argv[0], "delete")) {
        // 'delete' 명령어 실행
        if (argc != 2) {
            execute_help(argc, argv);
            return 0;
        }
        execute_delete(argc, argv);
    } else if (!strcmp(argv[0], "tree")) {
        // 'tree' 명령어 실행
        if (argc != 2) {
            execute_help(argc, argv);
            return 0;
        }
        execute_tree(argc, argv);
    } else if (!strcmp(argv[0], "help")) {
        // 'help' 명령어 실행
        execute_help(argc, argv);
    } else if (!strcmp(argv[0], "exit")) {
        // 'exit' 명령어 실행
        execute_exit(argc, argv);
        return 1;
    } else {
        fprintf(stderr, "wrong command in prompt\n");
        execute_help(argc, argv);
    }
    return 0;
}

int check_add_option(int argc, char *argv[]) {
	int i = 0;

    if(!strcmp(argv[2],"-t")) {
		while (argv[3][i])
		{
			if(argv[3][i] < '0' || argv[3][i] > '9')
				return false;
			i++;
		}
		execute_time = atoi(argv[3]);
	} else {
		return false;
	}
    return true;  // 옵션 검사 성공 시 반환
}

int except_add(char *path) {
	FILE *fp;
	char monitor_file[BUFLEN];
    if(access(path, F_OK) < 0 || get_filetype(path) != 0) {
        return 1;
    }
    if(access(monitor_list,F_OK) < 0) {
        if ((fp = fopen(monitor_list, "w+")) == NULL) {  // monitor_list.txt 파일 오픈
            fprintf(stderr, "%s open error\n", monitor_list);
            exit(1);
        }
    } else {
        if ((fp = fopen(monitor_list, "r+")) == NULL) {  // monitor_list.txt 파일 오픈
            fprintf(stderr, "%s open error\n", monitor_list);
            exit(1);
        }
    }
	while (fgets(monitor_file, sizeof(monitor_file), fp) != NULL) {  // monitor_list 파일에서 각 줄을 읽어옴
        *strchr(monitor_file, ' ') = '\0';
		if(strlen(path) >= strlen(monitor_file)) { // 이미 모니터링 중인 폴더이거니 포함되는 경우
			if(!strncmp(path, monitor_file, strlen(monitor_file))) {
                fprintf(stderr, "Directory already being monitored\n");
				fclose(fp);
				return 2;
			}
		} else {
			if(!strncmp(path, monitor_file, strlen(path))) {// 이미 모니터링 중인 폴더를 포함 하는 경우
                fprintf(stderr, "Directory already being monitored\n");
				fclose(fp);
				return 2;
			}
		}
    }
	fclose(fp);
    return 0;
}

void execute_add(int argc, char *argv[]) {
    char real_path[BUFLEN]; // 실제 경로를 저장할 변수
    time_t mn_time; // 모니터링 주기를 저장할 변수

    if (realpath(argv[1], real_path) == NULL) {
        // 주어진 경로의 실제 경로를 얻어오지 못한 경우
        execute_help(argc, argv);
        return;
    }
    int except_n = except_add(real_path);
    if (except_n == 1) { // 예외처리 모음
        execute_help(argc, argv);
        return;
    }
    if (except_n == 2) // 예외처리 모음
        return;
    init_daemon(real_path, mn_time); // 데몬 프로세스 초기화

    printf("monitoring started (%s)\n", real_path);
    return;
}

void execute_delete(int argc, char *argv[]) {
    FILE *fp;
    FILE *temp_fp;
    char monitor_info[BUFLEN];
    pid_t pid;
    int is_exist_pid = false;

    if(access(monitor_list,F_OK) < 0) {
        if ((fp = fopen(monitor_list, "w+")) == NULL) {  // monitor_list.txt 파일 오픈
            fprintf(stderr, "%s open error\n", monitor_list);
            exit(1);
        }
    } else {
        if ((fp = fopen(monitor_list, "r+")) == NULL) {  // monitor_list.txt 파일 오픈
            fprintf(stderr, "%s open error\n", monitor_list);
            exit(1);
        }
    }

    if ((temp_fp = fopen("temp.txt", "w")) == NULL) {  // temp.txt 파일을 쓰기 모드로 열기
        fprintf(stderr, "temp.txt open error\n");
        exit(1);
    }

    while (fgets(monitor_info, sizeof(monitor_info), fp) != NULL) {  // monitor_list 파일에서 각 줄을 읽어옴
        monitor_info[strlen(monitor_info) - 1] = '\0';  // 개행 문자 제거
        pid = atoi(strchr(monitor_info, ' ') + 1);  // 공백 다음에 오는 문자열을 pid로 변환하여 저장
        if (pid == atoi(argv[1])) {  // argv[1]에 전달된 pid와 일치하는 경우
            is_exist_pid = true;  // is_exist_pid 변수를 true로 설정
        } else {
            fprintf(temp_fp, "%s\n", monitor_info);  // temp_fp 파일에 monitor_info를 쓰기
        }
    }

    fclose(fp);  // monitor_list 파일 닫기
    fclose(temp_fp);  // temp_fp 파일 닫기

    if (is_exist_pid) {  // is_exist_pid가 true인 경우
		remove(monitor_list);  // 기존의 monitor_list 파일 삭제
    	rename("temp.txt", monitor_list);  // temp.txt 파일을 monitor_list 파일로 이름 변경
        kill_daemon(atoi(argv[1]), monitor_info);  // kill_daemon 함수 호출하여 해당 pid에 대응하는 데몬 프로세스 종료
    } else
		fprintf(stderr, "does not exist PID\n");
}


void kill_daemon(pid_t pid, char *dirpath) {
    if (kill(pid, SIGUSR1) == 0)
        printf("monitoring ended (%s)\n", dirpath);
}

void execute_tree(int argc, char *argv[]) {
    tree *root;
    struct stat filestat;

	if(except_tree(argv[1])) {
		fprintf(stderr, "This file is not being monitored\n");
		return ;
	}
	
    if (stat(argv[1], &filestat) == -1) {  // argv[1]에 전달된 경로의 파일 정보를 가져옴
        // stat 함수 호출에 실패한 경우
        fprintf(stderr, "stat error for %s\n", argv[1]);
        return;
    }

    root = create_node(argv[1], argv[1], filestat.st_mode, filestat.st_mtime);  // 루트 노드 생성
    make_tree(root, argv[1]);  // 파일 시스템 트리 생성
    print_tree(root, 0);  // 트리 출력
    free_tree(root);  // 트리 메모리 해제
}

int except_tree(char *path) {
	FILE *fp;
	char monitor_file[BUFLEN]="";
	char real_path[BUFLEN]="";

	if(realpath(path, real_path) == NULL)
		return 1;

	if(access(monitor_list,F_OK) < 0) {
        if ((fp = fopen(monitor_list, "w+")) == NULL) {  // monitor_list.txt 파일 오픈
            fprintf(stderr, "%s open error\n", monitor_list);
            exit(1);
        }
    } else {
        if ((fp = fopen(monitor_list, "r+")) == NULL) {  // monitor_list.txt 파일 오픈
            fprintf(stderr, "%s open error\n", monitor_list);
            exit(1);
        }
    }

	while (fgets(monitor_file, sizeof(monitor_file), fp) != NULL) {  // monitor_list 파일에서 각 줄을 읽어옴
		*strchr(monitor_file, ' ') = '\0';
		if (!strcmp(monitor_file, real_path)) {// 현재 모니터링 중인 파일이라면
			fclose(fp);
			return 0;
		}
    }
	fclose(fp);
    return 1;
}

void execute_help(int argc, char *argv[]) {
	printf("Usage:\n");
	printf(" > add <DIRPATH> [OPTION]\n");
	printf("  -t : The daemon process restarts monitoring at <TIME> intervals\n");
	printf(" > delete <DAEMON_PID>\n");
	printf(" > tree <DIRPATH>\n");
	printf(" > help\n");
	printf(" > exit\n");
}

void execute_exit(int argc, char *argv[]) {
	exit(0);
}

// 데몬 프로세스 초기화
void init_daemon(char *dirpath, time_t mn_time) {
    pid_t pid, dpid;
    FILE *fp, *fp2;
    DIR *dir;
    tree *old, *new;
    struct stat filestat;
    struct dirent *entry;
    struct stat file_stat;
    char filename[BUFLEN] = {0};
    char logPath[BUFLEN] = {0};
    char timestamp[20];

    snprintf(logPath, sizeof(logPath), "%s/log.txt", dirpath);  // 로그 파일 경로 생성

    if ((pid = fork()) < 0) {  // 자식 프로세스 생성
        fprintf(stderr, "fork error\n");
    } else if (pid == 0) {  // 자식 프로세스인 경우
        if ((dpid = make_daemon()) < 0) {  // 데몬 프로세스 생성
            fprintf(stderr, "make_daemon error\n");
            exit(1);
        }

        if ((fp = fopen(monitor_list, "a+")) == NULL) {  // monitor_list.txt 파일 오픈
            fprintf(stderr, "%s open error\n", monitor_list);
            exit(1);
        }
        fprintf(fp, "%s %d\n", dirpath, dpid);  // 모니터링 정보를 monitor_list.txt에 기록
        fflush(fp);
        fclose(fp);

        if ((fp2 = fopen(logPath, "a+")) == NULL) {  // log.txt 파일 오픈
            fprintf(stderr, "%s open error\n", logPath);
            exit(1);
        }

        new = create_node(dirpath, dirpath, filestat.st_mode, filestat.st_mtime);  // 새로운 노드 생성
        make_tree(new, dirpath);  // 파일 시스템 트리 생성

        while (!signal_received) {
            old = new;
            new = NULL;

            sleep(execute_time);  // 일정 시간 대기

            new = create_node(dirpath, dirpath, filestat.st_mode, filestat.st_mtime);  // 새로운 노드 생성
            make_tree(new, dirpath);  // 파일 시스템 트리 생성

            compare_tree(old, new, fp2);  // 이전 트리와 새로운 트리 비교하여 변경 사항 기록
            free_tree(old);  // 이전 트리의 메모리 해제
        }

        fclose(fp2);  // 로그 파일 닫기
    } else {
        return;  // 부모 프로세스는 함수 실행을 종료하고 반환
    }
}


pid_t make_daemon(void) {
	pid_t pid;
	int fd, maxfd;

	if ((pid = fork()) < 0) { // 자식 프로세스 생성
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid != 0) // 부모 프로세스는 종료
		exit(0);

	setsid(); // 새 세션 생성

	// 터미널 제어 시그널 무시
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	// 현재 열려있는 파일 디스크립터들을 닫음
	maxfd = getdtablesize();
	for (fd = 0; fd < maxfd; fd++) //for debug , fd=3
		close(fd);
	
	umask(0); // 파일 생성 권한 변경
	//chdir("/");
	// stdin, stdout, stderr를 /dev/null로 재지정하여 모든 입출력을 버림
	fd = open("/dev/null", O_RDWR);
	dup(0);
	dup(0);
	
	return getpid(); // 데몬 프로세스의 PID 반환
}

tree *create_node(char *path, char *fileName, mode_t mode, time_t mtime) {
	tree *new;

	new = (tree *)malloc(sizeof(tree));
	strcpy(new->path, path);
	strcpy(new->fileName, fileName);
	new->mode = mode;
	new->mtime = mtime;
	new->next = NULL;
	new->prev = NULL;
	new->child = NULL;
	new->parent = NULL;

	return new;
}

void make_tree(tree *dir, char *path) {
	struct dirent **filelist;
	int count;
	int i;

	if ((count = scandir(path, &filelist, scandir_filter, alphasort)) < 0) {
		fprintf(stderr, "scandir error for %s\n", path);
		return;
	}
	
	for (i = 0; i < count; i++) {
		char filepath[BUFLEN];
		struct stat filestat;
		tree *new_node;

		// 파일의 전체 경로 생성
        snprintf(filepath, sizeof(filepath), "%s/%s", path, filelist[i]->d_name);

		// 파일의 상태 정보 가져오기
        if (stat(filepath, &filestat) == -1) {
            fprintf(stderr, "stat error for %s\n", filepath);
            continue;
        }

		// 새로운 노드 생성
        new_node = create_node(filepath, filelist[i]->d_name, filestat.st_mode, filestat.st_mtime);

		// 생성한 노드를 링크드 리스트에 추가
        if (dir->child == NULL) {
            // 현재 디렉토리의 첫 번째 자식 노드로 추가
            dir->child = new_node;
        } else {
            // 현재 디렉토리의 마지막 자식 노드를 찾아서 추가
            tree *last_child = dir->child;
            while (last_child->next != NULL) {
                last_child = last_child->next;
            }
            last_child->next = new_node;
            new_node->prev = last_child;
        }

		// 부모-자식 관계 설정
        new_node->parent = dir;

        // 만약 현재 파일이 디렉토리라면, 재귀적으로 하위 디렉토리 탐색
        if (S_ISDIR(filestat.st_mode))
            make_tree(new_node, filepath);
	}
	// 스캔한 파일 목록 메모리 해제
    for (i = 0; i < count; i++)
        free(filelist[i]);
    free(filelist);
}

void compare_tree(tree *old, tree *new, FILE *fp) {
    if (old == NULL && new == NULL)
        return;  // 이전 트리와 새로운 트리가 모두 없는 경우, 함수 종료

    else if (old == NULL && new != NULL) {
        // 이전 트리에 없고 새로운 트리에 있는 경우 (새로운 파일 생성)
        if (S_ISREG(new->mode)) {
            printFormattedTime(new->mtime, fp, 0);
            fprintf(fp, "[create][%s]\n", new->path);
            fflush(fp);  // 파일 버퍼 비우기
        }
        compare_tree(NULL, new->next, fp);  // 다음 노드로 이동
        compare_tree(NULL, new->child, fp);  // 자식 노드로 이동
    }

    else if (old != NULL && new == NULL) {
        // 이전 트리에 있고 새로운 트리에 없는 경우 (파일 삭제)
        if (S_ISREG(old->mode)) {
            printFormattedTime(old->mtime, fp, 1);
            fprintf(fp, "[remove][%s]\n", old->path);
            fflush(fp);  // 파일 버퍼 비우기
        }
        compare_tree(old->next, NULL, fp);  // 다음 노드로 이동
        compare_tree(old->child, NULL, fp);  // 자식 노드로 이동
    }

    else {
        int compare = strcmp(old->path, new->path);

        if (compare == 0) {
            // 이전 트리와 새로운 트리에 같은 경로가 있는 경우
            if (old->mtime != new->mtime) {
                // 수정 시간이 다른 경우 (파일 수정)
                if (S_ISREG(new->mode)) {
                    printFormattedTime(new->mtime, fp, 0);
                    fprintf(fp, "[modify][%s]\n", new->path);
                    fflush(fp);  // 파일 버퍼 비우기
                }
            }
            compare_tree(old->next, new->next, fp);  // 다음 노드로 이동
            compare_tree(old->child, new->child, fp);  // 자식 노드로 이동
        }

        else if (compare < 0) {
            // 이전 트리의 경로가 새로운 트리의 경로보다 작은 경우 (파일 삭제)
            if (S_ISREG(old->mode)) {
                printFormattedTime(old->mtime, fp, 1);
                fprintf(fp, "[remove][%s]\n", old->path);
                fflush(fp);  // 파일 버퍼 비우기
            }
            compare_tree(old->next, new, fp);  // 다음 노드로 이동
            compare_tree(old->child, NULL, fp);  // 자식 노드로 이동
        }

        else {
            // 이전 트리의 경로가 새로운 트리의 경로보다 큰 경우 (새로운 파일 생성)
            if (S_ISREG(new->mode)) {
                printFormattedTime(new->mtime, fp, 0);
                fprintf(fp, "[create][%s]\n", new->path);
                fflush(fp);  // 파일 버퍼 비우기
            }
            compare_tree(old, new->next, fp);
			compare_tree(NULL, new->child, fp);
        }
    }
}

void print_tree(tree *node, int depth) {
    if (node == NULL) return;

    // 들여쓰기로 깊이를 표시
    for (int i = 1; i < depth; i++) {
        printf("|   ");
    }

    // 현재 노드 정보 출력
	if(depth != 0) printf("|-- ");
    printf("%s\n", node->fileName);

    // 자식 노드 출력 (재귀 호출)
    print_tree(node->child, depth + 1);

    // 형제 노드 출력 (재귀 호출)
    print_tree(node->next, depth);
}

void free_tree(tree *cur) {
    if (cur == NULL) return;
    // 자식 노드 재귀적으로 해제
    free_tree(cur->child);
    // 현재 노드 해제
    free(cur);
    // 형제 노드 재귀적으로 해제
    free_tree(cur->next);
}

int scandir_filter(const struct dirent *file) {
	if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..")
			|| !strcmp(file->d_name, "log.txt")
			|| !strcmp(file->d_name, monitor_list)) {
		return 0;
	}
	else
		return 1;
}

// 인자로 들어온 문자열을 문자로 분할하여 배열에 저장 후 크기 반환
int splitByCh(char dest[][FILELEN], char *str, char *ch) {
    int i = 0;
    char *temp;
    char *new_str = (char *)malloc(strlen(str)+1);
    strcpy(new_str,str);

    temp = strtok(new_str, ch);
    // strtok 함수가 NULL을 반환할때까지
    while (temp!= NULL)
    {
        if(i>5)
            return -1;
        strcpy(dest[i],temp);
        i++;
        temp = strtok(NULL, ch);
    }
    return i;
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

void printFormattedTime(time_t mtime, FILE *fp, int is_delete) {
    char buffer[80];
    struct tm *timeinfo;

    if (is_delete)
        mtime = time(NULL); // 현재 시간으로 설정
    
    timeinfo = localtime(&mtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    fprintf(fp,"[%s]",buffer);
	fflush(fp); // 파일 버퍼 비우기
}

int main(int argc, char *argv[]) {
    ssu_monitor(argc, argv);
}