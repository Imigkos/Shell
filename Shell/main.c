#include "header.h"

char *user;

int main()
{
    char *input_array;
    char **parsed_input = malloc(BUFSIZ * sizeof(char));
    start_shell(); // initialize shell
    while (1)
    {
        print_directory();
        input_array = getInput();
        if (strcasecmp(input_array, "exit") == 0)
        { // exit shell if user types exit
            printf("\n NOW EXITING SHELL");
            return 1;
        }

        parsed_input = parseInput(input_array);
        execute_commands(parsed_input);
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
    fprintf(stderr, "\n%s@strahd:%s$ ", user, dir);
}

char *getInput() // read character until EOF or \n is reached and store in array
{
    int input_array_size = 1024;
    int index = 0;
    char *input_array = malloc(sizeof(char) * input_array_size);
    int current;

    if (!input_array) // if error during allocation
    {
        fprintf(stderr, "Error when creating input array\n");
        exit(0);
    }

    while (1)
    {
        current = getchar();
        if (current == '\n' || current == EOF) // put trailing newline after EOF
        {
            input_array[index] = '\0';
            return input_array;
        }
        else // store current character in array
        {
            input_array[index] = current;
        }
        index++;
    }
}

char **parseInput(char *input)
{

    char *token = strtok(input, " "); // tokenizes input stored in input_array using space, first word sorted in token
    char **tokens = malloc(BUFSIZ * sizeof(char));
    // array of individual words from input stored as arguments
    int position = 0; // number of arguments

    if (!tokens)
    {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    while (token != NULL)
    {
        tokens[position++] = token;
        token = strtok(NULL, " ");
    }

    token = strtok(NULL, " ");

    tokens[position] = NULL;
    return tokens;
}

int execute_commands(char **parsed_input)
{

    if (strcmp(parsed_input[0], "cd") == 0)
    {
        if (chdir(parsed_input[1]) != 0)
        {
            perror("");
            
        }
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        printf("Forking child failed\n");
        return -1;
    }
    else if (pid == 0)
    {
        if (execvp(parsed_input[0],parsed_input) == -1)
        {
            perror("shell");
        }
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
    return 1;
}