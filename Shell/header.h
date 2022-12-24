#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define clear() printf("\033[H\033[J")
#define BUFSIZE 1024


void start_shell();
void print_directory();
char* getInput();
char** parseInput(char *input);
int execute_commands(char **parsed_input);