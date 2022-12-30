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

    command *command_arr = malloc(sizeof(command) * BUFSIZE); // array of commands
                                                              // array of arguments for each command

    int position = 0;
    int command_count = 0;
    command_arr[command_count].arguments = malloc(sizeof(char) * BUFSIZE);
    int found_pipe = 0;
    int found_redirect = 0;

    for (i = 0; i < strlen(input); i++)
    { // command_arr[0]          ca[0]   ca[1]
        if (input[i] == input[i + 1] && input[i] == '>')
        {
            found_redirect = 1;
        }
        if (input[i] == '|')
        {
            found_pipe = 1;
        }
        if (input[i] == '>' && input[i - 1] != '>' && input[i + 1] == ' ')
        {
            found_redirect = 2;
        }
        if (input[i] == '<')
        {
            found_redirect = 3;
        }
    }

    switch (found_redirect)
    {
    case 1:
        char *buffer = strtok(input, ">>"); // parse command
        buffer[strlen(buffer) - 1] = '\0';  // cut trailing whitespace
        char *file = strtok(NULL, ">> ");   // parse filename
        redirectOutput(file, buffer, O_WRONLY | O_CREAT | O_APPEND);
        return NULL;
        break;
    case 2:
        buffer = strtok(input, ">");
        buffer[strlen(buffer) - 1] = '\0';
        file = strtok(NULL, "> ");
        redirectOutput(file, buffer, O_WRONLY | O_CREAT | O_TRUNC);
        return NULL;
    case 3:
        buffer = strtok(input, "<");
        buffer[strlen(buffer) - 1] = '\0';
        file = strtok(NULL, "< ");
        redirectInput(file, buffer);
        return NULL;
        break;
    }

    if (found_pipe == 1)
    {
        parsePipes(input);
        return NULL;
    }

    char *token = strtok(input, " ");

    while (token != NULL)
    {

        command_arr[command_count].arguments[position++] = token;
        token = strtok(NULL, " ");
    }

    token = strtok(NULL, " ");

    command_arr[command_count].arguments[position] = NULL;
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
    parsed_input = NULL;
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
        dup2(out, STDOUT_FILENO);
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

int redirectOutput(char *filename, char *buffer, int flags)
{
    command *commandArray;
    int output;
    int out = open(filename, flags, 0600);

    if (out < 0)
    {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    output = dup(STDOUT_FILENO);
    if (dup2(out, STDOUT_FILENO) < 0)
    {
        fprintf(stderr, "Error redirecting\n");
        return 1;
    }

    commandArray = parseInput(buffer);
    if (commandArray != NULL)
    {
        execute_commands(commandArray[0].arguments);
    }
    fflush(stdout);

    close(out);
    dup2(output, STDOUT_FILENO);
    close(output);
}

int redirectInput(char *filename, char *buffer)
{
    command *commandArray;
    int in = open(filename, O_RDONLY);

    if (in < 0)
    {
        fprintf(stderr, "Error opening file\n");
        return -1;
    }

    dup2(in, 0);

    commandArray = parseInput(buffer);
    if (commandArray != NULL)
    {
        execute_commands(commandArray[0].arguments);
    }
    fflush(stdin);

    close(in);
    return 1;
}
