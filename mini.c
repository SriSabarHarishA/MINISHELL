/*
Name       : Sri Sabari Harish A      Reg No: 25026_091
Date       : 26-05-2026
Title      : MINISHELL
Description: MiniShell is a simple Linux shell developed in C that can execute built-in and external commands. 
             It supports commands like cd, pwd, echo, and exit, along with external Linux commands using fork(), 
             execvp(), and waitpid(). The project helps in understanding Linux system programming, process management, 
             and shell implementation concepts.
*/

#include "mini.h"
 
char *get_command(char *input_string)
{
    int cmd_index = 0;
 
    while (input_string[cmd_index] != ' ' &&
           input_string[cmd_index] != '\0')
    
        cmd_index++;
 
    input_string[cmd_index] = '\0';
 
    return input_string;   
}
 
void extract_external_commands(char external_commands[200][20])
{
    int  fd;
    char buff;            
    char line_buf[20];    
    int  row   = 0;       
    int  col   = 0;       
 
    fd = open("external.txt", O_RDONLY);
    if (fd < 0)
    {
        printf("Error: external.txt not found\n");
        external_commands[0][0] = '\0';
        return;
    }
 
    while (read(fd, &buff, sizeof(buff)) > 0)   
    {
        if (buff == '\n')
        {
           
            line_buf[col] = '\0';
 
            if (col > 0)   
            {
                strncpy(external_commands[row], line_buf, 19);
                external_commands[row][19] = '\0';
                row++;
            }
 
            col = 0;       
        }
        else
        {
            if (col < 19)  
            {
                line_buf[col] = buff;
                col++;
            }
        }
    }
 
    if (col > 0 && row < 200)
    {
        line_buf[col] = '\0';
        strncpy(external_commands[row], line_buf, 19);
        external_commands[row][19] = '\0';
        row++;
    }
 
    if (row < 200)
        external_commands[row][0] = '\0';
 
    close(fd);   
}
 
int parse_input_to_2d(char *input, char cmd[100][100])
{
    int row=0, col=0, i=0;     
 
    while (input[i] != '\0')
    {
        if (input[i] == ' ')
        {
            if (col > 0)          
            {
                cmd[row][col] = '\0';
                row++;
                col = 0;
            }
        }
        else
        {
            cmd[row][col++] = input[i];
        }
        i++;
    }
 
    if (col > 0)
    {
        cmd[row][col] = '\0';
        row++;
    }
 
    if (row < 100)
        cmd[row][0] = '\0';
 
    return row;   
}

int check_command_type(char *command, char *builtins[], char external_commands[200][20])
{
    // Check if builtin 
    for (int i = 0; builtins[i] != NULL; i++)
    {
        if (strcmp(command, builtins[i]) == 0)
            return BUILTIN;
    }
 
    // Check if external 
    for (int i = 0; external_commands[i][0] != '\0'; i++)
    {
        if (strcmp(command, external_commands[i]) == 0)
            return EXTERNAL;
    }
 
    return NO_COMMAND;
}

int execute_external(char *args[], int token_count)
{
    pid_t pid;
    int   status;
 
    // NULL-terminate args for execvp
    char *exec_args[101];
    for (int i = 0; i < token_count; i++)
        exec_args[i] = args[i];
    exec_args[token_count] = NULL;
 
    pid = fork();
 
    if (pid < 0)
    {
        perror("fork failed");
        return -1;
    }
    else if (pid == 0)
    {
        // CHILD: replace with the requested program 
        execvp(exec_args[0], exec_args);
 
        //Only reached if execvp fails 
        printf("msh: %s: execution failed\n", exec_args[0]);
        exit(1);
    }
    else
    {
        // PARENT: wait for child to finish
        waitpid(pid, &status, 0);
 
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
    }
 
    return 0;
}

int execute_builtin(char cmd[100][100], int token_count, int last_exit_status)
{
    // exit
    if (strcmp(cmd[0], "exit") == 0)
    {
        printf("Exiting minishell...\n");
        exit(0);
    }
 
    else if (strcmp(cmd[0], "pwd") == 0)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf("%s\n", cwd);
        else
            perror("pwd failed");
    }
 
    else if (strcmp(cmd[0], "cd") == 0)
    {
        if (token_count < 2)
        {
            char *home = getenv("HOME");
            if (home != NULL)
                chdir(home);
        }
        else
        {
            if (chdir(cmd[1]) != 0)
                printf("msh: cd: %s: No such file or directory\n", cmd[1]);
        }
    }
 
    // echo — also handles $?, $$, $SHELL 
    else if (strcmp(cmd[0], "echo") == 0)
    {
        for (int i = 1; i < token_count; i++)
        {
            if (strcmp(cmd[i], "$?") == 0)
                printf("%d", last_exit_status);
            else if (strcmp(cmd[i], "$$") == 0)
                printf("%d", getpid());
            else if (strcmp(cmd[i], "$SHELL") == 0)
                printf("%s", getenv("SHELL") ? getenv("SHELL") : "/bin/msh");
            else
                printf("%s", cmd[i]);
 
            if (i < token_count - 1)
                printf(" ");
        }
        printf("\n");
    }
 
    return 0;
}


int main()
{
    char prompt[100]      = "minishell> ";   
    char input[100];
    char cmd[100][100];
    char external_command[200][20];
    int  last_exit_status = 0;

    char *builtins[] = {
        "echo", "printf", "read", "cd", "pwd","pushd", "popd", "dirs", "let", "eval","set", "unset", 
        "export", "declare", "typeset","readonly", "getopts", "source", "exit", "exec",
        "shopt", "caller", "true", "type", "hash","bind", "help", NULL
    };
 
    system("clear");
    extract_external_commands(external_command);
 
    while (1)
    {
        //print prompt
        printf("%s", prompt);
        fflush(stdout);
 
        //read i/p
        int bytes = read(0, input, sizeof(input) - 1);
        if (bytes <= 0)
            break;                    
 
        input[bytes] = '\0';         
 
        int len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
            input[len - 1] = '\0';
 
        //empty i/p
        if (input[0] == '\0')
            continue;
 
        if (strncmp(input, "PS1=", 4) == 0)
        {
            char *new_prompt = input + 4;

            if (strchr(new_prompt, ' ') != NULL)
            {
                printf("ERROR: space not allowed in prompt\n");
            }
            else
            {
                strcpy(prompt, new_prompt);
            }
            continue;
        }
 
        int token_count = parse_input_to_2d(input, cmd);  //Parse into 2D array
 
        // printf("[DEBUG] token_count = %d\n", token_count);
        // for (int i = 0; i < token_count; i++)
        //     printf("  cmd[%d] = \"%s\"\n", i, cmd[i]);

        //check command
        int type = check_command_type(cmd[0], builtins, external_command);
        
        //Execute
        if (type == BUILTIN)
        {
            //printf("BUILTIN command: %s\n", cmd[0]);
            execute_builtin(cmd, token_count, last_exit_status);
        }
            
 
        else if (type == EXTERNAL)
        {
            char *args[101];
            for (int i = 0; i < token_count; i++)
                args[i] = cmd[i];
 
            last_exit_status = execute_external(args, token_count);
            //printf("EXTERNAL command: %s\n", cmd[0]);
        }
 
        else
            printf("msh: %s: command not found\n", cmd[0]);
            last_exit_status = 127;
    }
 
    return 0;
}