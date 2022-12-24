#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define clear() printf("\033[H\033[J")
#define BUFSIZE 1024

#define BUILTIN_NONE 0
#define BUILTIN_APPEND 1
#define BUILTIN_OVERWRITE 2
#define BUILTIN_PIPE 3

void start_shell();
void print_directory();
char* getInput();
char** parseInput(char *input, int *builtin);
int execute_commands(char **parsed_input,  int builtin, char *file);