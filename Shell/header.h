#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#define clear() printf("\033[H\033[J")


void start_shell();
void print_directory();
char* getInput();
