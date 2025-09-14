#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMDS 17
#define MAX_CMD_LEN 1024

void execute_pipeline(char* input) {
    char *commands[MAX_CMDS];
    int num_cmds = 0;

    // Remove starting and ending quotes if present
    if (input[0] == '"' && input[strlen(input) - 1] == '"') {
        input[strlen(input) - 1] = '\0';
        input++;
    }

    // Split the input string by '|' character (pipeline separator)
    char* token = strtok(input, "|");
    while (token != NULL && num_cmds < MAX_CMDS) {
        while (*token == ' ') token++; // skip leading spaces
        commands[num_cmds++] = token; // stores each command in to commands[] array
        token = strtok(NULL, "|");
    }

    if (token != NULL) {
        printf("Error\n");
        exit(1);
    }

    int prev_fd = -1;

    // pipe() is used between commands so the output of becomes the input of the next 
    for (int i = 0; i < num_cmds; i++) {
        int pipe_fd[2];
        if (i != num_cmds - 1) {
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                printf("Error\n");
                exit(1);
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            printf("Error\n");
            exit(1);
        }
        else if (pid == 0) {

            // if not first command, read from prev_fd
            if (i != 0) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            // If not last command, write to pipe_fd[1]
            if (i != num_cmds - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            //Tokenize command into argv
            char *argv[100];
            int arg_idx = 0;
            char *arg = strtok(commands[i], " ");
            while (arg != NULL) {
                argv[arg_idx++] = arg;
                arg = strtok(NULL, " ");
            }
            argv[arg_idx] = NULL;

            execvp(argv[0], argv);
            printf("Error\n");
            exit(1);
        }
        else {
            if (prev_fd != -1)
                close(prev_fd);
            if (i != num_cmds - 1) {
                close(pipe_fd[1]);
                prev_fd = pipe_fd[0];
            }
        }
    }

    // Wait for all children
    for (int i = 0; i < num_cmds; i++) {
        wait(NULL);
    }
}

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error\n");
        return 1;
    }

    execute_pipeline(argv[1]);
    return 0;
}
