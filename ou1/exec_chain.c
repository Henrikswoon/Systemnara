#include "exec_chain.h"
#define BUFFER_SIZE 1024

void iterate(command* c){
    printf("%s", c->argv[0]);

    pid_t p;
    int fd[2];          //Pipe
    int prev_fd = -1;   //Read end of previous Pipe

    while(c != NULL){
        //Create a pipe, given that there is a 'next' to pipe the result to.
        if(c->next != NULL){
            if(pipe(fd) == -1){
                perror("pipe: ");
                deAllocate_command(c);
                exit(EXIT_FAILURE);
            }
        }
        if((p = fork()) < 0){
            perror("fork: ");
            deAllocate_command(c);
            exit(EXIT_FAILURE);
        } else if ( p == 0 ) {
            //CHILD PROCESS
            if(prev_fd != -1){
                dup2(prev_fd, STDIN_FILENO);   // Redirect input 
                close(prev_fd);                 
            }
            if(c->next != NULL) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }

            close(fd[0]);
            execvp(c->name, c->argv);
            perror("execvp: ");
            exit(EXIT_FAILURE);
        } else if (p > 0){
            //PARENT PROCESS
            if(prev_fd != -1){
                close(prev_fd);
            }

            if(c->next != NULL){
                close(fd[1]);
                prev_fd = fd[0];
            }

            if(waitpid(p, NULL, 0) != p){
                perror("Waitpid: ");
                exit(EXIT_FAILURE);
            }
        }

        command* tmp = c;
        c = c -> next;
        deAllocate_command_node(tmp);
    }
}

/*
void _write(int fd, char* message){
    int len = strlen(message);
    write(fd, &len, sizeof(len));
    write(fd, message, len);
}
*/
/*
char* _read(int fd) {
}

void _exec(command* c, int fd){
    dup2(fd, STDOUT_FILENO); // Redirect standard output to pipe 
    execvp(c->name,c->argv);
    */
    /*
    pid_t p;
    p = fork();
    if(p < 0){
        perror("fork:");
        exit(EXIT_FAILURE);
    } else if(p == 0){
    //CHILD PROCESS//
        execvp(c->name, c->argv);
    //CHILD PROCESS//
    } else return; //PARENT SHOULD RETURN AND CLEAN UP :)
}
    */