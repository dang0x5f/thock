#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
    bool end_session = false;
    SessionTask task = SESSION_NOTASK;

    if(!initialize_program()){
        fprintf(stderr,"initialize_program()\n");
        exit(EXIT_FAILURE);
    }

    if(!initialize_interface()){
        fprintf(stderr,"initialize_interface()\n");
        exit(EXIT_FAILURE);
    }

    draw();
    while(!end_session){
        task = retrieve_session_task();
        switch(task){
            case SESSION_NOTASK:
                /* draw(); */
                break;
            case SESSION_RUN:
                /* end_session = thock(); */
                thock();
                break;
            case SESSION_STATS:
                //show_stats();
                break;
            case SESSION_MODULES:
                //view_modules();
                break;
            case SESSION_END:
                end_session = true;
                break;
        }
    }
    
    exit_program();

    /* show_stats(); */

    exit(EXIT_SUCCESS);
}



// ----------------------------------------------------------------

/*
    int num_of_mods = sizeof(modules)/sizeof(Module);
    printf("\t%d %d\n", sizeof(modules), sizeof(Module));

    for(int x = 0; x < num_of_mods; x++){
        words = modules[x].get_wordset(&size);
        printf("%s\n",words);
        free(words);
    }

    if(words != NULL) free(words);
*/
