#include "create_chain_from_file.h"
#include "exec_chain.h"
int main(int argc, char* argv[]){
    //---Construct LL---//
    command* c; 
    int len = 0;
    switch(argc){
        case 1:
            c = parse_from_stdin(&len);
            break;
        case 2:
            c = parse_from_argv(&len, argv);
            break;

        default: // If argc is neither 1 || 2 then 'perror'.
            perror("Args: ");
            exit(EXIT_FAILURE);
        break;
    }
    if(c == NULL){
        printf("c is null");
        exit(EXIT_FAILURE);
    }

/*
    command* tmp = c;
    while(tmp != NULL) {
        if(tmp->argv[1] != NULL) {
            printf("%s %s %s\n", tmp->name, tmp->argv[0], tmp->argv[1]);
        } else {
            printf("%s %s\n", tmp->name, tmp->argv[0]);
        }
        tmp = tmp->next;
    }tmp = NULL;
*/
    

    //---Construct LL---//

    //---Execute the command chain---//
    //iterate(c);    
    //---Execute the command chain---//

    //---de-allocation---//
    wait(NULL);
    if ((deAllocate_command(c)) == 0){
        printf("Deallocated command chain successfully!\n");
    }else{
        printf("Memory couldn't be successfully deallocated.\n");
    } 
    //---de-allocation---//

    return 0;
}
