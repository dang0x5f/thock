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

/* TODO: press enter to continue at game completion */ 
/* TODO: timer */
