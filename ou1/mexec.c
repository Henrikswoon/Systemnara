#include <stdio.h>
#include <stdlib.h>
#include "mexec.h"

int main(int argc, char* argv[2]){

    switch(argc){
        case 1:
            break;
        case 2:
            break;

        default: // If argc is neither 1 || 2 then 'perror'.
            perror("Args: ");
            exit(EXIT_FAILURE);
        break;
    }
}
