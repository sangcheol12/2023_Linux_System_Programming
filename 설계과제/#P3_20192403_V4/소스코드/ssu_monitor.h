#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

#define FILELEN 128
#define BUFLEN 1024

typedef struct tree
{
    char path[BUFLEN];
    char fileName[FILELEN];
    mode_t mode;
    time_t mtime;
    struct tree *next;
    struct tree *prev;
    struct tree *child;
    struct tree *parent;
} tree;

void ssu_monitor(int argc, char *argv[]);
void ssu_prompt(void);
int execute_command(int argc, char *argv[]);

void execute_add(int argc, char *argv[]);
int check_add_option(int argc, char *argv[]);
int except_add(char *path);

void execute_delete(int argc, char *argv[]);
void kill_daemon(pid_t pid, char *dirpath);

void execute_tree(int argc, char *argv[]);
int except_tree(char *path);

void execute_help(int argc, char *argv[]);

void execute_exit(int argc, char *argv[]);

void init_daemon(char *dirpath, time_t mn_time);
pid_t make_daemon(void);

tree *create_node(char *path, char *filename, mode_t mode, time_t mtime);
void make_tree(tree *dir, char *path);
void compare_tree(tree *old, tree *new, FILE *fp);
void print_tree(tree *node, int depth);
void free_tree(tree *cur);

int scandir_filter(const struct dirent *file);

int splitByCh(char dest[][FILELEN], char *str, char *ch);
int get_filetype(char *path);
void printFormattedTime(time_t mtime, FILE *fp, int is_delete);