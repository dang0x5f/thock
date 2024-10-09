#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <errno.h>

#include "ui.h"
#include "util.h"
#include "mods.h"

void request_wordset(uint8_t* wordset_state, uint32_t* wordset_length)
{
    wchar_t* wordset = NULL;
    int size = 0;

    if(wordset) free(wordset);

    wordset = modules[0].get_wordset(&size);    

    if(wordset == NULL){
        exit_ncurses();
        fprintf(stderr, "Error allocating wordset: %s\n", strerror(errno));
        exit(0);
    }

    *wordset_length = size;
    wordset_state = malloc((*wordset_length) * sizeof(uint8_t));

    load_wordset_textview(wordset,size,wordset_state);
    free(wordset);

}

void init_wordset_state(wchar_t* wordset, uint8_t* wordset_state, uint32_t length)
{
    uint32_t index = 0;
    wchar_t* iter;
    
    iter = wordset; 
    while(index < length){
        if(index == 0)
            *(wordset_state+index) = CURSOR;
        else if((wint_t)*iter == KEY_SPC)
            *(wordset_state+index) = UNLOCKED;
        else
            *(wordset_state+index) = EMPTY;

        index++;
        iter = wordset + index;
    }
}
