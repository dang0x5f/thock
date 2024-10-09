#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "thock.h"
#include "ui.h"
#include "mods.h"

enum char_status {
    CURSOR,
    EMPTY,
    CORRECT,
    INCORRECT,
    LOCKED,
    UNLOCKED
};

int request_wordset(wchar_t*);

int main(void)
{
    int size = 0;
    wchar_t key = 0;
    wchar_t* wordset = NULL;
    int* wordset_state = NULL;

    init_ncurses();
    
    size = request_wordset(wordset);
    while(1){
        key  = get_key();

    }

    exit_ncurses();

    return(0);
}

int request_wordset(wchar_t* wordset)
{
    int size = 0;

    if(wordset) free(wordset);

    wordset = modules[0].get_wordset(&size);    

    if(wordset == NULL){
        exit_ncurses();
        fprintf(stderr, "Error allocating wordset: %s\n", strerror(errno));
        exit(0);
    }

    load_wordset_textview(wordset,size);
    /* TODO: newlines = size / width; */ 
    /* TODO: format temp_wordset -> wordset, free temp_wordset */
    free(wordset);

    return size;
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
