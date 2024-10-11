#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "util.h"

/* typedef struct { */
/* best_wpm */
/* avg_wpm */
/* total_words */
/* total_rounds */
/* }Statistics; */

/* Global Variable */
/* Statistics stat_sheet; */

int main(void)
{
    if(!initialize_program()){
        fprintf(stderr,"initialize_program()\n");
        exit(EXIT_FAILURE);
    }

    if(!initialize_interface()){
        fprintf(stderr,"initialize_interface()\n");
        exit(EXIT_FAILURE);
    }

/*
    load();

    while(!end_session){
        task = retieve_task();
        switch(task){
            case SESSION_RUN:
                run();
            case SESSION_STATS:
                //show_stats();
                break;
            case SESSION_MODULES:
                //view_modules();
                break;
            case SESSION_END:
                end_session = 0;
                break;
        }
    }
*/
    
    /* show_stats(); */

    exit(EXIT_SUCCESS);
}





// ----------------------------------------------------------------


/* int main(void) */
/* { */
/*     int action = 0; */
/*     int size = 0; */
/*     uint8_t* wordset_state = NULL; */
/*     uint32_t wordset_length = 0; */

/*     init_ncurses(); */
    
/*     request_wordset(wordset_state, &wordset_length); */
/*     while(1){ */
/*         action = get_key(wordset_state); */
/*     } */

/*     return(0); */
/* } */

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
