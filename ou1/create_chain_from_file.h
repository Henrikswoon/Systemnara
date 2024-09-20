#ifndef CREATE_CHAIN_FROM_FILE_H
#define CREATE_CHAIN_FROM_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <string.h>
#include <unistd.h>
    
    /*
        Linked list containing the commands to be executed.
        struct command* next;
        const char* name;
        const char** argv;
    */
    typedef struct command {    
        struct command * next;
        char* name;
        char** argv;
    } command;

    // Used when input is piped into stdin through './mexec < FILE
    command* parse_from_stdin(int* len);

    //Used when input is given as an argument through './mexec FILE'
    command* parse_from_argv(int* len, char* argv[2]);

    //Wrapped by parser functions, creates a LL of command structs
    command* command_chain_from_file(int* len, FILE *f, command* previous );

    void cleanup_command_chain_from_file(char* line, char** argv);

    command* allocate_command(char* name, char** argv);
    int deAllocate_command(command* c);
    int deAllocate_command_node(command* c);

#endif