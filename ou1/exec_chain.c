#include "exec_chain.h"
#define BUFFER_SIZE 1024

void iterate(command* c){
    int fd[2];
    pid_t p;
    while(c != NULL){
        if(pipe(fd) == -1){
            perror("pipe: ");
            deAllocate_command(c);
            exit(EXIT_FAILURE);
        }
        if((p = fork()) < 0){
            perror("fork: ");
            deAllocate_command(c);
            exit(EXIT_FAILURE);
        }else if(p == 0){ 
        //CHILD PROCESS//    
            deAllocate_command(c); // Deallocate the copy of 'c'
            close(fd[1]); //Close Write
            char* buffer = _read(fd[0]); 
            close(fd[0]); //Close Read
            printf("from buffer: %s\n", buffer);
            free(buffer);
            exit(EXIT_SUCCESS);
        //CHILD PROCESS//
        }else if(p > 0){
        //PARENT PROCESS//
            close(fd[0]); //Close Read
            _exec(c, fd[1]);
            close(fd[1]); //Close Write
            if (waitpid(p, NULL, 0) != p){
                perror("waitpid: ");
                exit(EXIT_FAILURE);
            }
        //PARENT PROCESS//
        } 
        //ITERATE
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

char* _read(int fd) {
    size_t current_buffer_size  = BUFFER_SIZE;
    size_t total_read           = 0;

    char* buffer = calloc(BUFFER_SIZE, sizeof(char));
    if(buffer == NULL){
        perror("calloc: ");
        exit(EXIT_FAILURE);
    }

    int i = 2;
    int nread;
    //buffer+read uses pointer arithmetic to append!
    while((nread = read(fd, buffer+total_read, BUFFER_SIZE)) > 0){
        if(nread == -1){
            perror("read: ");
            free(buffer);
            exit(EXIT_FAILURE);
        }
        total_read += nread;
        if(total_read >= current_buffer_size){
            current_buffer_size *= i;
            char* tmp = realloc(buffer, current_buffer_size);
            if(tmp == NULL){
                perror("realloc: ");
                free(buffer);
                exit(EXIT_FAILURE);
            }
            buffer = tmp;
        }
        i++;
    }
    if(total_read > 0){
        buffer[total_read] = '\0';
    }
    else{
        free(buffer);
        return NULL;
    }
    return buffer;
}

void _exec(command* c, int fd){
    dup2(fd, STDOUT_FILENO); // Redirect standard output to pipe 
    execvp(c->name,c->argv);
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
    */
}