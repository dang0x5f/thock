/* #include <stdlib.h> */
/* #include <errno.h> */
#include <stdint.h>

#include "ui.h"
#include "util.h"

int main(void)
{
    int action = 0;
    int size = 0;
    uint8_t* wordset_state = NULL;
    uint32_t wordset_length = 0;

    init_ncurses();
    
    request_wordset(wordset_state, &wordset_length);
    while(1){
        action = get_key(wordset_state);
        
/*
        switch(action){
            case QUIT_ACTION:
                back out;
                break;
            case REQUEST_ACTION:
                request new wordset;
                break;
            case CONTINUE_ACTION:
                check wordset_state;
                break;
        }
*/

    }

    return(0);
}

/*
int main2(void)
{
    char* words = NULL;
    int size = 0;

    int num_of_mods = sizeof(modules)/sizeof(Module);
    printf("\t%d %d\n", sizeof(modules), sizeof(Module));

    for(int x = 0; x < num_of_mods; x++){
        words = modules[x].get_wordset(&size);
        printf("%s\n",words);
        free(words);
    }

    if(words != NULL) free(words);

    return(0);    
}
*/
