#include "header.h"

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
    char dir[1024];
    getcwd(dir, sizeof(dir));
    fprintf(stderr, "%s@strahd:%s$ ",user, dir);
}

char* getInput()//read character until EOF or \n is reached and store in array
{
    int input_array_size = 1024;
    int index = 0;
    char *input_array = malloc(sizeof(char) * input_array_size);
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
