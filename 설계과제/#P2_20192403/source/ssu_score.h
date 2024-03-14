#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h> // 나중에 지우기
#include <fcntl.h>
#include <signal.h>
#ifndef MAIN_H_
#define MAIN_H_

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef STDOUT
	#define STDOUT 1
#endif
#ifndef STDERR
	#define STDERR 2
#endif
#ifndef TEXTFILE
	#define TEXTFILE 3
#endif
#ifndef CFILE
	#define CFILE 4
#endif
#ifndef OVER
	#define OVER 5
#endif
#ifndef WARNING
	#define WARNING -0.1
#endif
#ifndef ERROR
	#define ERROR 0
#endif

#define FILELEN 128
#define BUFLEN 1024
#define SNUM 100
#define QNUM 100
#define ARGNUM 5

struct ssu_scoreTable{
	char qname[FILELEN];
	double score;
};

typedef struct wrongList
{
	char fileName[FILELEN];
	double score;
	struct wrongList *next;
} wrongList;


typedef struct scoreList
{
	char *studentName;
	double totalScore;
	struct scoreList *next;
} scoreList;

void ssu_score(int argc, char *argv[]);
int check_option(int argc, char *argv[]);
void print_usage();

void score_students();
double score_student(int fd, char *id);
void write_first_row(int fd);

char *get_answer(int fd, char *result);
int score_blank(char *id, char *filename);
double score_program(char *id, char *filename);
double compile_program(char *id, char *filename);
int execute_program(char *id, char *filname);
pid_t inBackground(char *name);
double check_error_warning(char *filename);
int compare_resultfile(char *file1, char *file2);

void readScoreFile(char scoreArr[][BUFLEN]);
void writeSortedFile(char scoreArr[][BUFLEN]);
void bubbleSortScore();
void reverseBubbleSortScore();
void bubbleSortStdid();
void reverseBubbleSortStdid();
void do_sOption();
void do_mOption(char *ansDir);
int is_exist(char (*src)[FILELEN], char *target);

int is_thread(char *qname);
void redirection(char *command, int new, int old);
int get_file_type(char *filename);
void rmdirs(const char *path);
void to_lower_case(char *c);

void set_scoreTable(char *ansDir);
void read_scoreTable(char *path);
void make_scoreTable(char *ansDir);
void write_scoreTable(char *filename);
void set_idTable(char *stuDir);
int get_create_type();

void sort_idTable(int size);
void sort_scoreTable(int size);
void get_qname_number(char *qname, int *num1, int *num2);
int get_s_mode(char *mode);
int get_s_mode2(int argc, char *argv[]);
char *getExt(char* filename);
void appendWrongList(wrongList **wrongRoot, char *fileName, double score);
void printWrongList(wrongList *wrongRoot);
void FreeWrongList(wrongList *wrongRoot);
void appendScoreList(char *studentName, double totalScore);
void freeScoreList();

#endif
