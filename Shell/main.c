#include "header.h"

char *user;

int main()
{
    char *input_array;
    char **parsed_input = malloc(BUFSIZE * sizeof(char));
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

        parsed_input = parseInput(input_array, &builtin);
        execute_commands(parsed_input, builtin, file);
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

char **parseInput(char *input, int *builtin)
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
        if (strcmp(token, ">>") == 0)
        {
            *builtin = BUILTIN_APPEND;
            break;
        }
        else if (strcmp(token, ">") == 0)
        {
            *builtin = BUILTIN_OVERWRITE;
            break;
        }
        else if (strcmp(token, "|") == 0)
        {
            *builtin = BUILTIN_PIPE;
            break;
        }
        tokens[position++] = token;
        token = strtok(NULL, " ");
    }

    token = strtok(NULL, " ");

    tokens[position] = NULL;
    return tokens;
}

int execute_commands(char **parsed_input, int builtin, char *file)
{
    int status;
    pid_t pid;

    // Handle append and overwrite built-in commands
    if (builtin == BUILTIN_APPEND || builtin == BUILTIN_OVERWRITE)
    {
        int flags = O_CREAT | O_WRONLY;
        if (builtin == BUILTIN_APPEND)
        {
            flags |= O_APPEND;
        }
        else
        {
            flags |= O_TRUNC;
        }
        int fd = open(file, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        if (fd == -1)
        {
            perror("shell");
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    // Handle pipe built-in command
    if (builtin == BUILTIN_PIPE)
    {
        // Split command and arguments into left and right sides of the pipe
        char **left = parsed_input;
        char **right = &parsed_input[position + 1];

        // Create a pipe
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("shell");
            return -1;
        }
        // cd command
        if (strcmp(parsed_input[0], "cd") == 0)
        {
            if (chdir(parsed_input[1]) != 0)
            {
                perror("");
            }
            return 1;
        }

        // Fork a new process for the left side of the pipe
        pid = fork();
        if (pid < 0)
        {
            // Error forking
            printf("Forking child failed\n");
            return -1;
        }
        else if (pid == 0)
        {
            // Child process
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            if (execvp(left[0], left) == -1)
            {
                perror("shell");
            }
            exit(EXIT_FAILURE);
        }
        else
        {
            // Parent process
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(right[0], right) == -1)
            {
                perror("shell");
            }
            exit(EXIT_FAILURE);
        }
    }
    
    // Handle all other commands
    pid = fork();
    if (pid < 0)
    {
        // Error forking
        printf("Forking child failed\n");
        return -1;
    }
    else if (pid == 0)
    {
        // Child process
        if (execvp(parsed_input[0], parsed_input) == -1)
        {
            perror("shell");
        }
    }
    else
    {
        // Parent process
        //waitpid(pid, NULL, 0);
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}