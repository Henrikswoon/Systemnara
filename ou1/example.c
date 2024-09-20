#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct Command {
    char *cmd;
    char **args;
    struct Command *next;
} Command;

void execute_pipeline(Command *head) {
    int pipe_fd[2];
    int prev_pipe_fd = -1;
    
    Command *current = head;

    while (current != NULL) {
        if (pipe(pipe_fd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // Child process
            if (prev_pipe_fd != -1) {
                dup2(prev_pipe_fd, STDIN_FILENO);  // Redirect input from previous pipe
                close(prev_pipe_fd);
            }

            if (current->next != NULL) {
                dup2(pipe_fd[1], STDOUT_FILENO);  // Redirect output to current pipe
            }

            close(pipe_fd[0]);
            close(pipe_fd[1]);

            execvp(current->cmd, current->args);  // Execute the command
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {  // Parent process
            wait(NULL);  // Wait for the child process to finish

            close(pipe_fd[1]);  // Close the write end of the pipe in parent
            if (prev_pipe_fd != -1) {
                close(prev_pipe_fd);  // Close the previous pipe read end
            }

            prev_pipe_fd = pipe_fd[0];  // Save the read end of the current pipe
            current = current->next;
        }
    }
    
    // Final command needs to have its output written to the standard output
    if (prev_pipe_fd != -1) {
        close(prev_pipe_fd);
    }
}

int main() {
    // Create a linked list of commands
    // Example: `ls | grep txt | sort`
    Command cmd1 = {"ls", (char *[]){"ls", NULL}, NULL};
    Command cmd2 = {"grep", (char *[]){"grep", "txt", NULL}, NULL};
    Command cmd3 = {"sort", (char *[]){"sort", NULL}, NULL};
    cmd1.next = &cmd2;
    cmd2.next = &cmd3;

    execute_pipeline(&cmd1);

    return 0;
}
