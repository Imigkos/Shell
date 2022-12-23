#include "header.h"
#define BUFSIZE 1024

char *user;

int main()
{
    char *input_array;

    start_shell(); // initialize shell
    while (1)
    {
        print_directory(); 
        input_array = getInput();
        if (strcasecmp(input_array,"exit") == 0){ //exit shell if user types exit
            printf("\n NOW EXITING SHELL");
            return 1;
        }

        // uuuh strtok moment
        char *cmd = strtok(input_array, " "); //tokenizes input stored in input_array using space, first word sotred in cmd
        char *argv[BUFSIZE]; // array of individual words from input stored as arguments
        int argc = 0; //number of arguments
        while (cmd != NULL) {
            argv[argc++] = cmd; // stored current value of cmd in argv and increments argc
            cmd = strtok(NULL, " "); // calls strtok again to get next word, stores it in cmd
        }
        argv[argc] = NULL;

        // Execute the command
        execvp(argv[0], argv);

        printf("\nThats all there is for now ");
    }

}

void start_shell()
{

    clear(); // clear console
    printf("\n===========SHELL OPENED===========\n");
    user = getenv("USER"); // get username
    fprintf(stderr, "         Welcome %s\n", user);
    sleep(1);
}

void print_directory()
{
    char dir[BUFSIZE];
    getcwd(dir, sizeof(dir));
    fprintf(stderr, "%s@strahd:%s$ ",user, dir);
}

char* getInput()//read character until EOF or \n is reached and store in array
{
    //int input_array_size = 1024;
    int index = 0;
    char *input_array = malloc(sizeof(char) * BUFSIZE);
    int current;

    if (!input_array) //if error during allocation 
    {
        fprintf(stderr, "Error when creating input array\n");
        exit(0);
    }

    while (1)
    {
        current = getchar();
        if (current == '\n' || current == EOF)// put trailing newline after EOF
        {
            input_array[index] = '\0';
            return input_array;
        }
        else //store current character in array
        {
            input_array[index] = current; 
        }
        index++;
        
    }
}
