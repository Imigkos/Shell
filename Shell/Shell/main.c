#include "header.h"

char *user;

int main()
{
    char *input_array;
    command *parsed_input = malloc(BUFSIZ * sizeof(char));
    start_shell(); // initialize shell
    while (1)
    {
        print_directory();
        input_array = getInput();
        if (strcasecmp(input_array, "exit") == 0)
        { // exit shell if user types exit
            printf("NOW EXITING SHELL...\n");
            exit(0);
        }

        parsed_input = parseInput(input_array);
        if (parsed_input != NULL)
        {
            execute_commands(parsed_input[0].arguments);
        }
        free(parsed_input);
        parsed_input = NULL;
        free(input_array);
        input_array = NULL;
    }
}

void start_shell()
{

    clear(); // clear console
    printf("\n===========SHELL OPENED===========\n");
    user = getenv("USER"); // get username
    fprintf(stderr, "         Welcome %s\n", user);
}

void print_directory()
{
    char dir[1024];
    getcwd(dir, sizeof(dir));
    fprintf(stderr, "%s@strahd:%s$ ", user, dir);
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

command *parseInput(char *input)
{
    int i;
    // tokenizes input stored in input_array using space, first word sorted in token
    command *command_arr = malloc(sizeof(command) * BUFSIZE);
    command_arr[0].arguments = malloc(sizeof(char) * BUFSIZE);
    // array of individual words from input stored as arguments
    int position = 0; // number of arguments

    for (i = 0; i < strlen(input); i++)
    {

        if (input[i] == '|')
        {
            parsePipes(input);
            return NULL;
        }
    }
    char *token = strtok(input, " ");

    while (token != NULL)
    {

        command_arr[0].arguments[position++] = token;
        token = strtok(NULL, " ");
    }

    token = strtok(NULL, " ");

    command_arr[0].arguments[position] = NULL;
    return command_arr;
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

        if (execvp(parsed_input[0], parsed_input) == -1)
        {
            perror("shell");
            return -1;
        }
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
    free(parsed_input);
    return 1;
}

void parsePipes(char *input)
{
    int pipe_counter = 0;
    char *buffer;
    int found_pipe = 0;
    int argc;
    command *command_arr = malloc(sizeof(command) * BUFSIZ);

    buffer = strtok(input, " ");
    while (buffer != NULL)
    {
        if (strcmp(buffer, "|") == 0)
        {
            buffer = strtok(NULL, " ");
            found_pipe = 1;
        }
        else
        {
            argc = 0;
            command_arr[pipe_counter].arguments = malloc(sizeof(char) * BUFSIZ);
            command_arr[pipe_counter].arguments[argc++] = buffer;
            buffer = strtok(NULL, " ");
            while (buffer != NULL && strcmp(buffer, "|") != 0)
            {
                command_arr[pipe_counter].arguments[argc] = buffer;
                argc++;
                buffer = strtok(NULL, " ");
            }
            pipe_counter++;
            found_pipe = 0;
        }
        if (!found_pipe)
        {
            buffer = strtok(NULL, " ");
        }
    }
    command_arr[pipe_counter].arguments = malloc(sizeof(char) * BUFSIZ);
    command_arr[pipe_counter].arguments[0] = NULL;

    executePipes(command_arr);
    free(command_arr);
    command_arr = NULL;
}

void executePipes(command *commandArray)
{
    int fd[2];
    pid_t p1;
    int out;
    

    if (pipe(fd) < 0)
    {
        printf("\nError while initializing pipe");
        return;
    }
    p1 = fork();

    if (p1 < 0)
    {
        printf("\n Failed forking child");
    }

    if (p1 == 0)
    {
        out = dup(STDOUT_FILENO);
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);

        

        if (execute_commands(commandArray[0].arguments) < 0)
        {
            printf("\nERRRRRORRRRRR");
            exit(0);
        }

        close(fd[1]);
        dup2(out,STDOUT_FILENO);
        close(out);
       
    }
    else
    {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);

        if (execute_commands(commandArray[1].arguments) < 0)
        {
            printf("\nERRRRRORRRRRR");
            exit(0);
        }
        waitpid(p1, NULL, 0);
    }
}

