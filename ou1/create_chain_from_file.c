#include "create_chain_from_file.h"
command* parse_from_stdin(int* len) {
    if(isatty(fileno(stdin))){
        printf("stdin wasn't piped file correctly.");
        exit(EXIT_FAILURE);
    }
    
    FILE *f = stdin;
    if(f == NULL){
        printf("Provided file was NULL, exiting...");
        exit(EXIT_FAILURE);
    }
    command* c = command_chain_from_file(len, f, NULL); 
    return c; 
}

//MUST ASSIGN AND ALLOCATE 
//c
//  c->next
//  c->name
//  c->argv
//      c->argv[n]
command* allocate_command(char* name, char** argv){
    if(name == NULL){
        printf("'name' token was null");
        exit(EXIT_FAILURE);
    }    

    command *c = calloc(1,sizeof(command));    
    c->argv = argv;
    c -> name = strdup(name);                              
    return c;
    
}

//Recursively deallocates the entire structure
//status: 1==fail,0==success
int deAllocate_command(command* c){
    while(c!=NULL){
        if(c->name != NULL){
            free(c->name);
            c->name = NULL;
        } 
        if(c -> argv != NULL){
            int i = 0;  
            while(c->argv[i] != NULL){
                free(c->argv[i]);
                c->argv[i] = NULL;
                i++;
            }
            free(c->argv);
            c->argv = NULL;
        }
        //STEP
        command* tmp = c;
        c = c->next;
        free(tmp);
    }
    return 0;
}

int deAllocate_command_node(command* c){
    if(c->name != NULL){
        free(c->name);
        c->name = NULL;
    }
    if(c -> argv != NULL){
        int i = 0;  
        while(c->argv[i] != NULL){
            free(c->argv[i]);
            c->argv[i] = NULL;
            i++;
        }
        free(c->argv);
        c->argv = NULL;
    }
    free(c);
    return 0;
}

//See man 3 getline
//Passes a command chain to wrapper function,
//These command chains begin with a NULL pointer
//Following links in the chain point to the previous command 
//NULL <- First <- Second <- etc...
command* command_chain_from_file(int* len_commands, FILE *f, command* previous){
    (*len_commands)++;
    command* c;
    char    *line   = NULL; 
    size_t  len     = 0; 
    ssize_t nread   = 0;

    char** argv     = NULL;
    nread = getline(&line, &len, f); //line 80
    //Case 1: getLine causes an error, or reaches EOF
    remove_trailing_newline(line, nread); 
    if(nread == -1){
        if(feof(f)){
            cleanup_command_chain_from_file(line, NULL);
            return reverse_list(previous);
        } else {
            cleanup_command_chain_from_file(line, argv);
            perror("Reading from file: ");
            exit(EXIT_FAILURE);
        }
    }

    //Case 2: getLine assigns next line from 'f' to 'line'
    if(nread > 0){
        //Read a line and assign the first token as the command 'name'
        char* token = strtok(line, " ");
        printf("%s\n",token);
        if (token == NULL) {
            cleanup_command_chain_from_file(line, argv);
            perror("File must be formatted such that there are no empty rows: ");
            exit(EXIT_FAILURE);
        }
        char*  name = token; //Store the initial token as it represents c->name        

        //Following tokens should be arguments passed to the command
        int argc = 1; // First argument is always command name
        char** tmp = malloc(sizeof(char*));

        if(tmp == NULL){
            cleanup_command_chain_from_file(line, argv);
            perror("malloc: ");
            exit(EXIT_FAILURE);
        }

        tmp[0] = strdup(name);
        argv = tmp;
        while((token = strtok(NULL, " ")) != NULL){
            tmp = realloc(argv, (argc + 1) * sizeof(char*));

            if(tmp == NULL){
                cleanup_command_chain_from_file(line, argv);
                printf("Reallocation if 'argv' %d failed at 'command_chain_from_file', exiting...", argc);
                exit(EXIT_FAILURE);
            }

            argv = tmp;  
            argv[argc] = strdup(token);
            
            if(argv[argc] == NULL){
                cleanup_command_chain_from_file(line, argv);
                perror("Strdup: ");
                exit(EXIT_FAILURE);
            }

            argc++;
        }
        argv[argc+1] = NULL; // Causes issue
        c = allocate_command(name, argv);
        c -> next = previous;
        
        cleanup_command_chain_from_file(line, NULL);
        return command_chain_from_file(len_commands, f, c); 
    }
    return NULL; //ILLEGAL, SHOULDN'T BE HERE
}


//If argv shouldn't be freed, pass null instead
void cleanup_command_chain_from_file(char* line, char** argv){
    if(line != NULL){
        //printf("RUNNING CLEANUP... %s\n", line);
        free(line);
        line = NULL;
    }
    if(argv != NULL) {
        int i = 0;
        while(argv[i] != NULL){
            //printf("Freeing argv[%d]: %s\n", i, argv[i]);
            free(argv[i]);
            argv[i] = NULL;
            i++;
        } 
        free(argv);
        argv = NULL;
    }
}


command* parse_from_argv(int* len, char* argv[2]){
    FILE *f = fopen(argv[1], "r");
    if(f == NULL){
        perror("Error reading from file: ");
        exit(EXIT_FAILURE);
    }
    return command_chain_from_file(len, f, NULL);
}

command* reverse_list(command* c){
    command* prev = NULL;
    command* current = c;
    command* next = NULL;

    while(current != NULL){
        next = current->next;
        current->next=prev;
        prev = current;
        current = next;
    }
    return prev;
}

void remove_trailing_newline(char *str, ssize_t nread) {
    if (nread > 0 && str[nread - 1] == '\n') {
        str[nread - 1] = '\0';  // Replace the newline with a null terminator
    }
}