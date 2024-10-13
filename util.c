#include <stdio.h>
/* #include <stdlib.h> */
#include <locale.h>
/* #include <wchar.h> */
/* #include <errno.h> */
#include <stdbool.h>

#define ctrl(x)             ( (x) & 0x1F )

#include "ui.h"
#include "util.h"
#include "mods.h"

bool initialize_program(void)
{
    setlocale(LC_ALL,"");

    uint8_t mod_count = sizeof(modules)/sizeof(Module);

    if(!mod_count){
        fprintf(stderr,"no modules\n");
        return(false);
    }

    return(true);
}

bool initialize_interface(void)
{
    if(!initialize_stdscr()){
        fprintf(stderr,"initialize_stdscr() failed\n");
        return(false);
    }

    if(!initialize_wordset()){
        fprintf(stderr,"initialize_wordset() failed\n");
        return(false);
    }

    if(!initialize_textview()){
        fprintf(stderr,"initialize_textview() failed\n");
        return(false);
    }

    if(!initialize_prompt()){
        fprintf(stderr,"initialize_prompt() failed\n");
        return(false);
    }

    return(true);
}

SessionTask retrieve_task(void)
{
    SessionTask task = SESSION_NOTASK;

    wint_t keycode = get_keycode();
    switch(keycode){
        case ctrl('g'):
            if(!too_small())
                task = SESSION_RUN;
            break;
        case ctrl('x'):
            task = SESSION_END;
            break;
        case WEOF:
            // handled internal to ui
            break;
    }

    return(task);
}

/* TODO: probably will be returning task */
bool run(void)
{
    return(true);
}

/* TODO: bit field to decide which windows to draw */
void draw(void)
{
    draw_stdscr();
    draw_textview();
    draw_prompt();
}

void exit_program(void)
{
    free_wordset();
    free_textview();
    free_buffer();
    free_prompt();
    free_stdscr_exit();
}






// -----------------------------------------------------------------------------------

/* void request_wordset(uint8_t* wordset_state, uint32_t* wordset_length) */
/* { */
/*     wchar_t* wordset = NULL; */
/*     int size = 0; */

/*     if(wordset) free(wordset); */

/*     wordset = modules[0].get_wordset(&size); */    

/*     if(wordset == NULL){ */
/*         exit_ncurses(); */
/*         fprintf(stderr, "Error allocating wordset: %s\n", strerror(errno)); */
/*         exit(0); */
/*     } */

/*     *wordset_length = size; */
/*     wordset_state = malloc((*wordset_length) * sizeof(uint8_t)); */

/*     load_wordset_textview(wordset,size,wordset_state); */
/*     free(wordset); */

/* } */

/* void init_wordset_state(wchar_t* wordset, uint8_t* wordset_state, uint32_t length) */
/* { */
/*     uint32_t index = 0; */
/*     wchar_t* iter; */
    
/*     iter = wordset; */ 
/*     while(index < length){ */
/*         if(index == 0) */
/*             *(wordset_state+index) = CURSOR; */
/*         else if((wint_t)*iter == KEY_SPC) */
/*             *(wordset_state+index) = UNLOCKED; */
/*         else */
/*             *(wordset_state+index) = EMPTY; */

/*         index++; */
/*         iter = wordset + index; */
/*     } */
/* } */
