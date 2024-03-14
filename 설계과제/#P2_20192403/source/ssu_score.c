#include "ssu_score.h"
#include "blank.h"

extern struct ssu_scoreTable score_table[QNUM];
extern char id_table[SNUM][10];

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char errorDir[BUFLEN];
char **targetStudents=NULL;
char newScoreFile[BUFLEN];
char threadFiles[ARGNUM][FILELEN] = {0};
char iIDs[ARGNUM][FILELEN];
char *firstcTarget;
char *firstpTarget;
int s_mode1=0,s_mode2=0;
scoreList *scoreRoot = NULL;

int eOption = false;
int tOption = false;
int nOption = false;
int mOption = false;
int cOption = false;
int pOption = false;
int sOption = false;

void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];
	int i;

	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){ // -h 옵션이 존재하면 usage 출력 후 종료
			print_usage();
			return;
		}
	}
	if(get_s_mode2(argc, argv)) {
		fprintf(stderr, "Undefined s-option factor\n");
		exit(1);
	}

	memset(saved_path, 0, BUFLEN);
	if(argc >= 3){
		strcpy(stuDir, argv[1]);
		strcpy(ansDir, argv[2]);
	}

	if(!check_option(argc, argv))
		exit(1);

	if(cOption && pOption) {
		if(firstcTarget != NULL && firstpTarget != NULL) {
			fprintf(stderr, "Student Variable List must be one\n");
			exit(1);
		}
	}

	getcwd(saved_path, BUFLEN); // 현재 작업 경로를 받아 옴

	if(chdir(stuDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN); // 입력으로 들어 온 학생 디렉토리로 이동 후 현재 경로를 받아 옴

	chdir(saved_path);
	if(chdir(ansDir) < 0){
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN); // 입력으로 들어 온 정답 디렉토리로 이동 후 현재 경로를 받아 옴

	chdir(saved_path);

	set_scoreTable(ansDir); // 학생 점수 테이블 생성 (문제 번호, 점수)
	set_idTable(stuDir); // 학번 테이블 생성

	if((cOption || pOption) && (targetStudents != NULL)) {
		int is_exist;
		for(int i=0; targetStudents[i]; i++) {
			is_exist=0;
			for(int j=0; j < sizeof(id_table) / sizeof(id_table[0]); j++) {
				if(!strcmp(targetStudents[i],id_table[j]))
					is_exist = 1;
			}
			if(!is_exist) {
				fprintf(stderr, "%s student doesn't exist\n", targetStudents[i]);
				exit(1);
			}
		}
	}
	if(tOption && threadFiles[0][0]) {
		int is_exist;
		for(int i=0; threadFiles[i][0]; i++) {
			is_exist=0;
			for(int j=0; j < sizeof(score_table) / sizeof(score_table[0]); j++) {
				if(!strncmp(threadFiles[i],score_table[j].qname,strlen(threadFiles[i])))
					is_exist = 1;
			}
			if(!is_exist) {
				fprintf(stderr, "%s problem doesn't exist\n", threadFiles[i]);
				exit(1);
			}
		}
	}

	if(mOption)
		do_mOption(ansDir);

	printf("grading student's test papers..\n");
	score_students();
	if(sOption) {
		do_sOption();
		freeScoreList();
	}
	return;
}

char *retfirstTarget(int argc, char *argv[], char *mode) {
	char *ret;
	int i = 1;
	while (strcmp(argv[i],mode)) i++;
	if(i == argc-1 || argv[i+1][0] == '-') return NULL;
	i++;
	ret = calloc(sizeof(char), strlen(argv[i])+1);
	strcpy(ret,argv[i]);
	return ret;
}

char **retStudentList(int argc, char *argv[], char *startOpt) {
	int i = 1, j, student_cnt = 0;
	char **ret;

	while(strcmp(argv[i], startOpt)) i++;
	j = i;
	while (j<argc) {
		if(argv[j][0] == '-') break;
		j++;
	}
	ret = calloc(sizeof(char *),(j-i+1)); // index를 한칸 늘려서 NULL로 하여 while문을 쉽게 돌리기 위함
	while (i<j) {
		if(student_cnt < 5) {
			ret[student_cnt] = calloc(sizeof(char),strlen(argv[i])+1);
			strcpy(ret[student_cnt],argv[i]);
		}
		else
			printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
		student_cnt++;
		i++;
	}
	return ret;
}

int check_option(int argc, char *argv[])
{
	int i, j, k;
	int c;
	int exist = 0;

	while((c = getopt(argc, argv, "e:thn:mcps:1")) != -1)
	{
		switch(c){
			case 'e':
				eOption = true;
				strcpy(errorDir, optarg);

				if(access(errorDir, F_OK) < 0) {
					if(mkdir(errorDir, 0755) < 0) {
						fprintf(stderr,"%s can't make\n",errorDir);
						exit(1);
					}
				}
				else{
					rmdirs(errorDir);
					if(mkdir(errorDir, 0755) < 0) {
						fprintf(stderr,"%s can't make\n",errorDir);
						exit(1);
					}
				}
				break;
			case 't':
				tOption = true;
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM)
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else{
						strcpy(threadFiles[j], argv[i]);
					}
					i++; 
					j++;
				}
				break;
			case 'n':
				nOption = true;
				strcpy(newScoreFile, optarg);
				break;
			case 'm':
				mOption = true;
				break;
			case 'c':
				cOption = true;
				firstcTarget = retfirstTarget(argc,argv,"-c");
				if(firstcTarget)
					targetStudents = retStudentList(argc, argv, firstcTarget);
				break;
			case 'p':
				pOption = true;
				firstpTarget = retfirstTarget(argc,argv,"-p");
				if(firstpTarget)
					targetStudents = retStudentList(argc, argv, firstpTarget);
				break;
			case 's':
				s_mode1 = get_s_mode(optarg);
				sOption = true;
				break;
			case '1':
				break;
			case '?':
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}

void do_mOption(char *ansDir)
{
	double newScore;
	char modiName[FILELEN];
	char filename[FILELEN];
	char *ptr;
	int i;

	ptr = malloc(sizeof(char) * FILELEN);

	while(1){

		printf("Input question's number to modify >> ");
		scanf("%s", modiName);

		if(strcmp(modiName, "no") == 0)
			break;

		for(i=0; i < sizeof(score_table) / sizeof(score_table[0]); i++){
			strcpy(ptr, score_table[i].qname);
			ptr = strtok(ptr, ".");
			if(!strcmp(ptr, modiName)){
				printf("Current score : %.2f\n", score_table[i].score);
				printf("New score : ");
				scanf("%lf", &newScore);
				getchar();
				score_table[i].score = newScore;
				break;
			}
		}
	}

	sprintf(filename, "%s/%s", ansDir, "score_table.csv");
	write_scoreTable(filename);
	free(ptr);
}

void bubbleSortScore() { // 버블 정렬 방식으로 점수 기준으로 링크드리스트 정렬
    if (scoreRoot == NULL || scoreRoot->next == NULL) { // 리스트가 비어 있을 때
        return;
    }

    scoreList *ep = NULL;
    while (ep != scoreRoot->next) { // ep가 루트의 다음 노드와 같아질 때까지 반복
        scoreList *cp = scoreRoot, *prev = NULL;
		while (cp->next != ep) { // cp가 ep까지 반복
            scoreList *next = cp->next;
            if (cp->totalScore > next->totalScore) { // cp의 점수가 다음 노드의 점수보다 크면 두 노드의 위치를 바꿈
                if (prev != NULL) { // prev가 NULL이 아니면 cp의 위치를 next와 바꿈
                    prev->next = next;
                } else {
                    scoreRoot = next;
                }
                cp->next = next->next;
                next->next = cp;
                cp = next;
            }
            prev = cp;
            cp = cp->next;
        }
        ep = cp;
    }
}

void reverseBubbleSortScore() {
    if (scoreRoot == NULL || scoreRoot->next == NULL) {
        return;
    }

    scoreList *ep = NULL;
    while (ep != scoreRoot->next) {
        scoreList *cp = scoreRoot, *prev = NULL;
		while (cp->next != ep) {
            scoreList *next = cp->next;
            if (cp->totalScore < next->totalScore) {
                if (prev != NULL) {
                    prev->next = next;
                } else {
                    scoreRoot = next;
                }
                cp->next = next->next;
                next->next = cp;
                cp = next;
            }
            prev = cp;
            cp = cp->next;
        }
        ep = cp;
    }
}

void bubbleSortStdid() { // 버블 정렬 방식으로 점수 기준으로 링크드리스트 정렬
    if (scoreRoot == NULL || scoreRoot->next == NULL) {
        return;
    }

    scoreList *ep = NULL;
    while (ep != scoreRoot->next) {
        scoreList *cp = scoreRoot, *prev = NULL;
		while (cp->next != ep) {
            scoreList *next = cp->next;
            if (strcmp(cp->studentName, next->studentName) > 0) {
                if (prev != NULL) {
                    prev->next = next;
                } else {
                    scoreRoot = next;
                }
                cp->next = next->next;
                next->next = cp;
                cp = next;
            }
            prev = cp;
            cp = cp->next;
        }
        ep = cp;
    }
}

void reverseBubbleSortStdid() {
    if (scoreRoot == NULL || scoreRoot->next == NULL) {
        return;
    }

    scoreList *ep = NULL;
    while (ep != scoreRoot->next) {
        scoreList *cp = scoreRoot, *prev = NULL;
		while (cp->next != ep) {
            scoreList *next = cp->next;
            if (strcmp(cp->studentName, next->studentName) < 0) {
                if (prev != NULL) {
                    prev->next = next;
                } else {
                    scoreRoot = next;
                }
                cp->next = next->next;
                next->next = cp;
                cp = next;
            }
            prev = cp;
            cp = cp->next;
        }
        ep = cp;
    }
}

void readScoreFile(char scoreArr[][BUFLEN]) {
	FILE *fp;
	char scorePath[FILELEN];
	int idx=0;

	if(nOption)
		strcpy(scorePath, newScoreFile);
	else {
		strcpy(scorePath,ansDir);
		strcat(scorePath, "/score.csv");
	}

	if((fp = fopen(scorePath, "r")) == NULL){ // 파일 열기
		fprintf(stderr, "file open error for %s\n", scorePath);
		return ;
	}
	//fscanf(fp, "%s\n", scoreArr[0]);
	while(fscanf(fp, "%s\n", scoreArr[idx]) != EOF){ // 파일에서 각 라인마다 문제 이름과 점수를 읽어와서 배열에 저장
		idx++;
	}
	fclose(fp);
}

void writeSortedFile(char scoreArr[][BUFLEN]) {
	FILE *fp;
	char scorePath[FILELEN];
	int idx;
	scoreList *cp;

	if(nOption)
		strcpy(scorePath, newScoreFile);
	else {
		strcpy(scorePath,ansDir);
		strcat(scorePath, "/score.csv");
	}
	if(creat(scorePath, 0666) < 0) { // score.csv 파일 생성
		fprintf(stderr, "creat error for score.csv\n");
		return;
	}
	if((fp = fopen(scorePath, "w")) == NULL){ // 파일 열기
		fprintf(stderr, "file open error for %s\n", scorePath);
		return ;
	}
	fprintf(fp,"%s\n",scoreArr[0]);
	cp = scoreRoot;
	while(cp != NULL) {
		idx = 1;
		while(scoreArr[idx][0]) {
			if(!strncmp(cp->studentName, scoreArr[idx],strlen(cp->studentName))) {
				fprintf(fp,"%s\n",scoreArr[idx]);
				break;
			}
			idx++;
		}
		cp = cp->next;
	}
}

void do_sOption() {
	char scoreArr[SNUM+1][BUFLEN]={0};

	if (s_mode1 == 1) {
		if(s_mode2 == 1)
			bubbleSortStdid();
		else if(s_mode2 == -1)
			reverseBubbleSortStdid();
    } else if (s_mode1 == 2) {
		if(s_mode2 == 1)
			bubbleSortScore();
		else if(s_mode2 == -1)
			reverseBubbleSortScore();
    }
	readScoreFile(scoreArr);
	writeSortedFile(scoreArr);
}

int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM)
			return false;
		else if(!strcmp(src[i], ""))
			return false;
		else if(!strcmp(src[i++], target))
			return true;
	}
	return false;
}

void set_scoreTable(char *ansDir)
{
	char filename[FILELEN];

	sprintf(filename, "%s/%s", ansDir, "score_table.csv"); // 현재 작업 폴더 하위로 점수 테이블 경로 지정

	// check exist
	if(access(filename, F_OK) == 0)
		read_scoreTable(filename); // 파일에서 점수표 읽기
	else{
		if(mOption) {
			fprintf(stderr, "Score file does't exist\n"); // mOption시 점수 파일 없을 때
			exit(1);
		}
		make_scoreTable(ansDir); // 정답 디렉토리에서 파일을 읽어서 점수표 생성
		write_scoreTable(filename); // 점수표를 파일에 쓰기
	}
}

// 점수표 파일을 읽는 함수
void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;

	if((fp = fopen(path, "r")) == NULL){ // 파일 열기
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){ // 파일에서 각 라인마다 문제 이름과 점수를 읽어와서 배열에 저장
		strcpy(score_table[idx].qname, qname);
		score_table[idx++].score = atof(score);
	}

	fclose(fp);
}
// 파일 이름에 따라 스코어 테이블을 만드는 함수
void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp, *c_dirp;
	DIR *dp, *c_dp;
	char *tmp;
	int idx = 0;
	int i;
	// 텍스트 파일과 C 파일의 점수를 지정하는 방법을 입력받음
	num = get_create_type();
	// 빈칸 문제와 프로그램 문제의 점수를 입력받음
	if(num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore);
		printf("Input value of program question : ");
		scanf("%lf", &pscore);
	}
	// 정답 파일이 저장된 디렉토리 열기
	if((dp = opendir(ansDir)) == NULL){
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}
	// 디렉토리 내부를 검사하여 파일 이름을 score_table 배열에 저장
	while((dirp = readdir(dp)) != NULL){

		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		if((type = get_file_type(dirp->d_name)) < 0)
			continue;

		strcpy(score_table[idx].qname, dirp->d_name);

		idx++;
	}

	closedir(dp);
	sort_scoreTable(idx); // 파일 이름을 기준으로 score_table 배열 정렬

	// 정렬된 배열에 대해 파일의 종류를 확인하고 해당하는 점수를 입력받음
	for(i = 0; i < idx; i++)
	{
		type = get_file_type(score_table[i].qname);

		if(num == 1)
		{
			if(type == TEXTFILE)
				score = bscore;
			else if(type == CFILE)
				score = pscore;
		}
		else if(num == 2)
		{
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		score_table[i].score = score;
	}
}

// 점수표 파일을 쓰는 함수
void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]);

	if((fd = creat(filename, 0666)) < 0) { // 파일 생성
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	for(i = 0; i < num; i++)
	{
		if(score_table[i].score == 0) // 점수가 0일 경우 종료
			break;

		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		write(fd, tmp, strlen(tmp));
	}

	close(fd);
}

// 학생 디렉토리에서 학생 id를 추출하여 id_table에 저장하는 함수
void set_idTable(char *stuDir)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;

	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while((dirp = readdir(dp)) != NULL){
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		stat(tmp, &statbuf);

		if(S_ISDIR(statbuf.st_mode)) // 디렉토리인 경우 id_table에 추가
			strcpy(id_table[num++], dirp->d_name);
		else
			continue;
	}
	closedir(dp);

	sort_idTable(num); // id_table 정렬
}

// id_table을 오름차순으로 정렬하는 함수 
void sort_idTable(int size)
{
	int i, j;
	char tmp[10];

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			if(strcmp(id_table[j], id_table[j+1]) > 0){ // id_table을 오름차순으로 정렬
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}

// score_table 배열을 파일 이름의 번호순으로 정렬하는 함수
void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){
			// 파일 이름에서 번호 추출
			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);
			// 번호순으로 정렬
			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){

				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

// qname에서 문제 번호(num1)와 하위 문항 번호(num2)를 추출하는 함수
void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];

	strncpy(dup, qname, strlen(qname)); // qname을 dup에 복사
	*num1 = atoi(strtok(dup, "-.")); // '-' 또는 '.'으로 구분된 첫 번째 토큰을 추출하여 num1에 저장

	
	p = strtok(NULL, "-."); // '-' 또는 '.'으로 구분된 다음 토큰을 추출하여 p에 저장
	if(p == NULL)
		*num2 = 0; // p가 NULL인 경우, num2를 0으로 초기화
	else
		*num2 = atoi(p); // p가 NULL이 아닌 경우, p를 정수형으로 변환하여 num2에 저장
}

// 생성 유형을 선택하는 함수
int get_create_type()
{
	int num;

	while(1)
	{
		printf("score_table.csv file doesn't exist in TREUDIR!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		if(num != 1 && num != 2)
			printf("not correct number!\n"); // num이 1 또는 2가 아닌 경우, 오류 메시지 출력
		else
			break;
	}

	return num;
}

void score_students()
{
	double score = 0; // 전체 평균 점수를 계산하기 위한 변수
	int num;
	int fd;
	char tmp[BUFLEN];
	char scorePath[FILELEN];
	int size = sizeof(id_table) / sizeof(id_table[0]); // id_table 크기

	if(nOption) {
		strcpy(scorePath,newScoreFile);
		if(getExt(scorePath) == NULL || strcmp(getExt(scorePath),".csv")) {
			fprintf(stderr, "%s does not csv file\n", scorePath);
			exit(1);
		}
	}
	else {
		strcpy(scorePath,ansDir);
		strcat(scorePath, "/score.csv");
	}
	if((fd = creat(scorePath, 0666)) < 0){ // score.csv 파일 생성
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd); // 파일 첫 번째 행에 학생 ID와 문제 번호 출력

	for(num = 0; num < size; num++)
	{
		if(!strcmp(id_table[num], "")) // id_table에서 빈 문자열이 나온 경우, 반복문 종료
			break;

		sprintf(tmp, "%s,", id_table[num]); // 학생 ID를 tmp 문자열 버퍼에 저장하고 ','를 추가
		write(fd, tmp, strlen(tmp)); 
		score += score_student(fd, id_table[num]); // 학생의 점수를 계산하고, score에 추가
	}

	if(cOption)
		printf("Total average : %.2f\n", score / num); // 전체 평균 점수 출력

	printf("result saved.. (%s)\n",realpath(scorePath,NULL));
	if(eOption)
		printf("error saved.. (%s)\n",realpath(errorDir, NULL));
	close(fd);
}

// 학생 한 명의 점수를 계산하는 함수
double score_student(int fd, char *id)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);
	wrongList *wrongRoot;

	wrongRoot = NULL;
	for(i = 0; i < size ; i++)
	{
		// score_table 배열에서 더 이상 질문이 없을 경우 반복문을 빠져나옴
		if(score_table[i].score == 0)
			break;
		// 각 학생의 디렉토리에 있는 파일 경로를 생성하고 해당 파일이 존재하는지 확인
		snprintf(tmp, strlen(stuDir)+strlen(id)+strlen(score_table[i].qname)+3, "%s/%s/%s", stuDir, id, score_table[i].qname);
		if(access(tmp, F_OK) < 0) // 파일이 존재하지 않으면 false 반환
			result = false;
		else
		{
			// 파일 타입을 확인하고 각 타입에 맞게 점수를 계산하는 함수 호출
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}
		// 각 문제별로 점수를 계산해서 score 변수에 더하고, csv 파일에 저장
		if(result == false) {
			write(fd, "0,", 2);
			if(pOption)
				appendWrongList(&wrongRoot,score_table[i].qname,score_table[i].score);
		}
		else{
			if(result == true){ 
				score += score_table[i].score;
				sprintf(tmp, "%.2f,", score_table[i].score);
			}
			else if(result < 0){
				score = score + score_table[i].score + result;
				sprintf(tmp, "%.2f,", score_table[i].score + result);
			}
			write(fd, tmp, strlen(tmp));
		}
	}
	// 학생의 점수 계산이 끝나면 해당 학생의 이름과 총점을 출력하고, csv 파일에 저장
	printf("%s is finished..", id);
	if(cOption) {
		if(targetStudents == NULL) {
			printf(" score : %.2f",score);
			if(pOption)
				printf(",");
		}
		else {
			i = 0;
			while(targetStudents[i]) {
				if(!strcmp(id, targetStudents[i])) {
					printf(" score : %.2f",score);
					if(pOption)
						printf(",");
				}
				i++;
			}
		}
	}
	if(pOption) {
		if(targetStudents == NULL) {
			printf(" wrong problem : ");
			printWrongList(wrongRoot);
		}
		else {
			i = 0;
			while(targetStudents[i]) {
				if(!strcmp(id, targetStudents[i])) {
					printf(" wrong problem : ");
					printWrongList(wrongRoot);
				}
				i++;
			}
		}
	}
	printf("\n");
	sprintf(tmp, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));
	if(pOption)
		FreeWrongList(wrongRoot);
	if(sOption)
		appendScoreList(id, score);

	return score;
}

// csv 파일의 첫 번째 줄(문제 이름들)을 작성하는 함수
void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	write(fd, ",", 1);
	// score_table 배열에서 각 문제의 이름을 가져와서 csv 파일에 저장
	for(i = 0; i < size; i++){
		if(score_table[i].score == 0)
			break;
		
		sprintf(tmp, "%s,", score_table[i].qname);
		write(fd, tmp, strlen(tmp));
	}
	// 마지막에 총점을 저장하는 sum 열을 추가
	write(fd, "sum\n", 4);
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	memset(result, 0, BUFLEN);
	while(read(fd, &c, 1) > 0)
	{
		if(c == ':')
			break;
		
		result[idx++] = c;
	}
	if(result[strlen(result) - 1] == '\n')
		result[strlen(result) - 1] = '\0';

	return result;
}

// 학생의 답안 파일에서 정답을 읽어와 비교하는 함수
int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL; // 학생 답안 트리와 정답 트리
	int idx, start;
	char tmp[BUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN]; // 학생 답안과 정답 답안
	char qname[FILELEN]; // 문제 파일명이서 확장자를 제외한 부분
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	memset(qname, 0, sizeof(qname));
	// 파일명에서 확장자를 제외한 부분을 qname에 저장
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	snprintf(tmp, strlen(stuDir)+strlen(id)+strlen(filename)+3, "%s/%s/%s", stuDir, id, filename); // 학생 답안 파일 경로 생성
	fd_std = open(tmp, O_RDONLY);
	get_answer(fd_std, s_answer); // 학생 답안 파일에서 첫 번째 줄을 읽어와 s_answer에 저장

	if(!strcmp(s_answer, "")){ // 읽어온 학생 답안이 빈 문자열일 경우 종료
		close(fd_std);
		return false;
	}

	if(!check_brackets(s_answer)){ // 학생 답안에 괄호가 제대로 열고 닫혔는지 확인하는 함수 호출
		close(fd_std);
		return false;
	}

	strcpy(s_answer, ltrim(rtrim(s_answer))); // 학생 답안 문자열 앞뒤의 공백 제거

	if(s_answer[strlen(s_answer) - 1] == ';'){ // 학생 답안 문자열이 세미콜론으로 끝나는지 확인
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0'; // 세미콜론이 있으면 제거
	}

	if(!make_tokens(s_answer, tokens)){ // 학생 답안 문자열을 토큰으로 분리하는 함수 호출
		close(fd_std);
		return false;
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0); // 학생 답안을 이진 트리로 변환하여 std_root에 저장

	snprintf(tmp, strlen(ansDir)+strlen(filename)+2, "%s/%s", ansDir, filename);
	fd_ans = open(tmp, O_RDONLY);

	while(1)
	{
		ans_root = NULL;
		result = true;

		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		get_answer(fd_ans, a_answer); // 정답 파일에서 한 줄씩 읽어온다

		if(!strcmp(a_answer, "")) // 읽어온 내용이 빈 문자열인 경우 반복문 탈출
			break;
		
		strcpy(a_answer, ltrim(rtrim(a_answer))); // 문자열 좌우 공백 제거

		if(has_semicolon == false){ // 세미콜론 검사
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}
		
		else if(has_semicolon == true)
		{
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}
		// 정답 파일 내용을 토큰으로 분리하여 tokens 배열에 저장
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		ans_root = make_tree(ans_root, tokens, &idx, 0); // 정답 파일 내용을 이진 트리로 변환
		
		compare_tree(std_root, ans_root, &result); // 학생 답안과 정답 파일 내용을 비교하여 결과를 result에 저장

		if(result == true){ // 학생 답안이 정답일 경우 true 반환
			close(fd_std);
			close(fd_ans);

			if(std_root != NULL)
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root);
			return true;

		}
	}
	
	close(fd_std);
	close(fd_ans);

	if(std_root != NULL)
		free_node(std_root);
	if(ans_root != NULL)
		free_node(ans_root);

	return false;
}

// 학생 답안 파일을 컴파일하고 실행하여 점수를 계산하는 함수
double score_program(char *id, char *filename)
{
	double compile;
	int result;
	
	compile = compile_program(id, filename); // 학생 답안 파일 컴파일

	if(compile == ERROR || compile == false)
		return false;
	
	result = execute_program(id, filename); // 학생 답안 파일 실행

	if(!result)
		return false;

	if(compile < 0) // 컴파일 에러 발생 시, 에러 코드를 반환하여 처리
		return compile;

	return true;
}

// 스레드 문제인지 확인하는 함수
int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);

	if(tOption == true && strcmp(threadFiles[0],""))
		return true;
	for(i = 0; i < size; i++){
		if(!strcmp(threadFiles[i], qname))
			return true;
	}
	return false;
}

// 컴파일된 프로그램을 실행 파일로 만드는 함수
double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	// 파일 이름에서 확장자 제거
	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));
	// 파일이 스레드인지 확인
	isthread = is_thread(qname);
	// 정답 디렉토리에 있는 파일 경로와 실행 파일 경로 지정
	snprintf(tmp_f, strlen(ansDir)+strlen(filename)+2, "%s/%s", ansDir, filename);
	snprintf(tmp_e, strlen(ansDir)+strlen(qname)+6, "%s/%s.exe", ansDir, qname);
	// 스레드 옵션이 있으면 컴파일할 때 -lpthread 옵션 사용
	if(tOption && isthread)
		snprintf(command, strlen("gcc -o   -lpthread")+strlen(tmp_e)+strlen(tmp_f)+1,"gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		snprintf(command, strlen("gcc -o  ")+strlen(tmp_e)+strlen(tmp_f)+1,"gcc -o %s %s", tmp_e, tmp_f);
	// 컴파일 오류가 있는 경우, 오류 내용을 저장할 파일을 생성하고 오류 내용을 파일에 저장함
	snprintf(tmp_e, strlen("/_error.txt")+strlen(ansDir)+strlen(qname)+1,"%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);
	
	if(size > 0)
		return false;

	snprintf(tmp_f, strlen(stuDir)+strlen(id)+strlen(filename)+3, "%s/%s/%s", stuDir, id, filename);
	snprintf(tmp_e, strlen(stuDir)+strlen(id)+strlen(qname)+10,"%s/%s/%s.stdexe", stuDir, id, qname);

	if(tOption && isthread)
		snprintf(command, strlen("gcc -o   -lpthread")+strlen(tmp_e)+strlen(tmp_f)+1, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		snprintf(command, strlen("gcc -o  ")+strlen(tmp_e)+strlen(tmp_f)+1, "gcc -o %s %s", tmp_e, tmp_f);

	snprintf(tmp_f, strlen(stuDir)+strlen(id)+strlen(qname)+13, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	// 컴파일 오류가 있으면 처리 방법에 따라 오류 내용 출력 또는 파일에 저장함
	if(size > 0){
		// -e 옵션이 적용되어 있으면 학생별 오류 내용을 저장할 디렉토리를 생성하고 오류 내용을 해당 디렉토리의 파일에 저장함
		if(eOption)
		{
			snprintf(tmp_e, strlen(errorDir)+strlen(id)+2,"%s/%s", errorDir, id);
			if(access(tmp_e, F_OK) < 0) {
				if(mkdir(tmp_e, 0755) < 0) {
					fprintf(stderr,"%s is wrong id path\n",tmp_e);
					exit(1);
				}
			}

			snprintf(tmp_e, strlen(errorDir)+strlen(id)+strlen(qname)+13, "%s/%s/%s_error.txt", errorDir, id, qname);
			rename(tmp_f, tmp_e);

			result = check_error_warning(tmp_e); // 컴파일 오류 내용을 분석하여 반환값을 결정함
		}
		else{ 
			result = check_error_warning(tmp_f); // 컴파일 오류 내용을 분석하여 반환값을 결정함
			unlink(tmp_f);
		}

		return result;
	}

	unlink(tmp_f);
	return true;
}

// 컴파일 오류 및 경고 메시지를 확인하는 함수
double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	while(fscanf(fp, "%s", tmp) > 0){ // 파일의 내용을 한 단어씩 읽어들이면서
		if(!strcmp(tmp, "error:")) // "error:" 문자열이 있으면 ERROR 반환
			return ERROR;
		else if(!strcmp(tmp, "warning:")) // "warning:" 문자열이 있으면 WARNING에 1을 더함
			warning += WARNING;
	}

	return warning;
}

// 학생이 작성한 소스코드를 컴파일하고 실행 결과를 파일에 저장하는 함수
int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;
	// 문제 이름 추출
	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));
	// 정답 결과 파일 생성
	snprintf(ans_fname, strlen(ansDir)+strlen(qname)+9, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666);
	// 정답 소스코드를 실행하고 결과를 파일에 저장
	snprintf(tmp, strlen(ansDir)+strlen(qname)+6, "%s/%s.exe", ansDir, qname);
	redirection(tmp, fd, STDOUT);
	close(fd);
	// 학생 소스코드 실행 결과 파일 생성
	snprintf(std_fname, strlen(stuDir)+strlen(id)+strlen(qname)+10, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666);
	// 학생 소스코드를 백그라운드로 실행하고 결과를 파일에 저장
	snprintf(tmp, strlen("//.stdexe &")+strlen(stuDir)+strlen(id)+strlen(qname)+1, "%s/%s/%s.stdexe &", stuDir, id, qname);

	start = time(NULL);
	redirection(tmp, fd, STDOUT);
	// 일정 시간이 지나도 프로그램이 종료되지 않으면 프로세스를 강제 종료
	snprintf(tmp, strlen(qname)+8,"%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL);

		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname); // 실행 결과 파일 비교 값 반환
}

// 프로세스 이름을 인자로 받아 해당 프로세스가 백그라운드에서 실행 중인지 확인하는 함수
pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	// 백그라운드에서 실행 중인 프로세스 정보를 "ps" 명령어로 조회하고, 해당 정보를 "background.txt" 파일에 저장
	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	sprintf(command, "ps | grep %s", name);
	redirection(command, fd, STDOUT);
	// 파일에서 읽어온 정보가 없으면 0을 반환하고, 아니면 해당 프로세스의 pid를 추출하여 반환
	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	pid = atoi(strtok(tmp, " "));
	close(fd);

	unlink("background.txt");
	return pid;
}
// file1과 file2의 결과 파일을 비교하여 일치 여부를 판단하는 함수
int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		while((len1 = read(fd1, &c1, 1)) > 0){ // 파일 1에서 공백 문자가 나올 때까지 한 글자씩 읽는다.
			if(c1 == ' ') 
				continue;
			else 
				break;
		}
		while((len2 = read(fd2, &c2, 1)) > 0){ // 파일 2에서 공백 문자가 나올 때까지 한 글자씩 읽는다.
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		if(len1 == 0 && len2 == 0)
			break;

		to_lower_case(&c1); // 문자를 소문자로 변경한다.
		to_lower_case(&c2); // 문자를 소문자로 변경한다.

		if(c1 != c2){ // 두 문자가 다르면 파일이 일치하지 않는다.
			close(fd1);
			close(fd2);
			return false;
		}
	}
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	saved = dup(old); // old의 기존 파일 디스크립터를 saved에 복사
	dup2(new, old); // old의 파일 디스크립터를 new로 복사

	system(command); // command 명령어를 실행

	dup2(saved, old); // old의 파일 디스크립터를 saved로 복구
	close(saved);
}

int get_file_type(char *filename)
{
	char *extension = strrchr(filename, '.');

	if(!strcmp(extension, ".txt")) // 텍스트 파일이면 TEXTFILE 상수를 반환
		return TEXTFILE;
	else if (!strcmp(extension, ".c")) // C 파일이면 CFILE 상수를 반환
		return CFILE;
	else
		return -1; // 그 외의 경우에는 -1을 반환
}

void rmdirs(const char *path)
{
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char tmp[50];
	
	if((dp = opendir(path)) == NULL)
		return;

	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		snprintf(tmp, strlen(path)+strlen(dirp->d_name)+2, "%s/%s", path, dirp->d_name);

		if(lstat(tmp, &statbuf) == -1) // 파일의 정보를 가져옴
			continue;

		if(S_ISDIR(statbuf.st_mode)) // 파일이 디렉토리인 경우 재귀적으로 삭제
			rmdirs(tmp);
		else // 파일이 디렉토리가 아닌 경우 해당 파일을 삭제
			unlink(tmp);
	}

	closedir(dp);
	rmdir(path);
}

void to_lower_case(char *c)
{
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

void print_usage()
{
	printf("Usage : ssu_score <STD_DIR> <ANS_DIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -n <CSVFILENAME>\n");
	printf(" -m\n");
	printf(" -c [STUDENTIDS ...]\n");
	printf(" -p [STUDENTIDS ...]\n");
	printf(" -t [QNAMES ...]\n");
	printf(" -s <CATEGORY> <1|-1>\n");
	printf(" -e <DIRNAME>\n");
	printf("-f\n");
}

int get_s_mode(char *mode) {
	int ret = 0;
	if(!strcmp(mode, "stdid"))
		ret = 1;
	if(!strcmp(mode, "score"))
		ret = 2;
	return ret;
}

int get_s_mode2(int argc, char *argv[]) {
	int i = 1;
	while(i < argc) { // -s 옵션이 있는지 검사;
		if(!strcmp(argv[i], "-s"))
			break;
		i++;
	}
	if(i==argc) return 0;
	if(i + 2 >= argc) return 1;
	if(strcmp(argv[i+1],"stdid") && strcmp(argv[i+1],"score"))
		return 1;
	if(strcmp(argv[i+2], "1") && strcmp(argv[i+2], "-1"))
		return 1;
	s_mode2 = atoi(argv[i+2]);
	return 0;
}

char *getExt(char* filename)
{
    char* ptr = NULL;
 
    ptr = strrchr(filename, '.');
    if (ptr == NULL)
        return NULL;
 
    return ptr;
}

void appendWrongList(wrongList **wrongRoot, char *fileName, double score) {
	wrongList *newNode;
	wrongList *currentNode;

	newNode = malloc(sizeof(wrongList));

	strncpy(newNode->fileName,fileName,strlen(fileName)-strlen(getExt(fileName)));
	newNode->fileName[strlen(fileName)-strlen(getExt(fileName))] = 0;
	newNode->score = score;
	newNode->next = NULL;
	currentNode = *wrongRoot;
	if(currentNode == NULL) {
		*wrongRoot = newNode;
		return ;
	}
	while (currentNode->next != NULL) currentNode = currentNode->next;
	currentNode -> next = newNode;
	
}

void printWrongList(wrongList *wrongRoot) {
	wrongList *cp;

	cp = wrongRoot;
	while (cp != NULL)
	{
		printf("%s(%.2lg)",cp->fileName,cp->score);
		if(cp->next != NULL)
			printf(", ");
		cp = cp->next;
	}
}

void FreeWrongList(wrongList *wrongRoot) {
	wrongList *cp;

	cp = wrongRoot;
	while (cp != NULL)
	{
		free(cp);
		wrongRoot = wrongRoot->next;
		cp = wrongRoot;
	}
}

void appendScoreList(char *studentName, double totalScore) {
	scoreList *newNode;
	scoreList *currentNode;
	
	newNode = malloc(sizeof(scoreList));
	newNode->studentName = studentName;
	newNode->totalScore = totalScore;
	newNode->next = NULL;
	currentNode = scoreRoot;
	if(currentNode == NULL) {
		scoreRoot = newNode;
		return ;
	}
	while(currentNode->next != NULL) currentNode = currentNode->next;
	currentNode -> next = newNode;
}

void freeScoreList() {
	scoreList *cp;

	cp = scoreRoot;
	while(cp != NULL) {
		free(cp);
		scoreRoot = scoreRoot->next;
		cp = scoreRoot;
	}
}
