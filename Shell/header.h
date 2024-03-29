#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define clear() printf("\033[H\033[J")
#define BUFSIZE 1024

typedef struct command
{
   char **arguments;
} command;

void start_shell();
void print_directory();
char *getInput();
command *parseInput(char *input);
int execute_commands(char **parsed_input);
void parsePipes(char *input);
void executePipes(command *commandArray);
int redirectOutput(char *filename, char *buffer, int flags);
int redirectInput(char *filename, char *buffer);
