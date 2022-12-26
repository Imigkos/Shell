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
            printf("NOW EXITING SHELL...\n");
            exit(0);
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
    int i;
    // tokenizes input stored in input_array using space, first word sorted in token
    char **tokens = malloc(BUFSIZ * sizeof(char));
    // array of individual words from input stored as arguments
    int position = 0; // number of arguments

    if (!tokens)
    {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

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

        tokens[position++] = token;
        token = strtok(NULL, " ");
    }

    token = strtok(NULL, " ");

    tokens[position] = NULL;
    return tokens;
}

int execute_commands(char **parsed_input)
{

    if (parsed_input == NULL)
    { // pipe has already been executed so return
        return 0;
    }

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
        }
    }
    else
    {
        waitpid(pid, NULL, 0);
    }
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
            while (buffer!=NULL && strcmp(buffer, "|" ) != 0)
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
}

void executePipes(command *commandArray)
{

    // if (execvp(commandArray[1].cmd, commandArray[1].arguments) == -1)
    // {
    //     perror("shell: ");
    // }

    int fd[2];
    pid_t pid;
    if (pipe(fd) < 0)
    {
        printf("\nError while initializing pipe");
        return;
    }
    pid = fork();
    if (pid < 0)
    {
        printf("\n Failed forking child");
    }

    if (pid == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);

        // close(fd[1]);

        if (execvp(commandArray[0].arguments[0], commandArray[0].arguments) < 0)
        {
            printf("\nERRRRRORRRRRR");
            exit(0);
        }
    }
    else
    {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);

        if (execvp(commandArray[1].arguments[0], commandArray[1].arguments) < 0)
        {
            printf("\nERRRRRORRRRRR");
            exit(0);
        }
        else
        {
            wait(NULL);
            wait(NULL);
        }
    }
}
