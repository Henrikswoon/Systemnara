#include "create_chain_from_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//Overview
//0: Wait: Childprocess should wait until parent signals it's ready to write
//1: Read: Until pipe is closed !! Buffered if necessary
//2: Iterate: If there is a command -> next
//3: Execute: The command with the String passed from read
//4: Write: If necessary, do a Buffered write 
//5: Exit: Process should exit gracefully

// |p_1| closes it's write , |main| closes it's read, |main| closes it's write, signaling that |p_1| can begin propagation and execution
// nothing->|p_1|
// |p_1| then:
//          Opens a pipe
//          Opens a childprocess
//

//Iterate creates a new process which takes the previous output if any through pipe
void iterate(command* c);

//Reads from pipe until it's closed or timeout is reached
char* _read(int fd);

//Executes head of Command* 
void _exec(command* c, int fd);

//Writes to pipe, if necessary staggers the message
//void _write(int fd, char* m);

//Exits gracefully.
//void gexit();