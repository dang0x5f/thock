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

int request_wordset(char*);

int request_wordset(char* wordset)
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
    /* write_to_textview(wordset,size); */
    /* TODO: newlines = size / width; format temp_wordset -> wordset, free temp_wordset */

    return size;
}

int main2(void)
{
    char* words = NULL;
    int size = 0;
    
    /* words = modules[0].get_wordset(&size); */

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

int main(void)
{
    int size = 0;
    wchar_t key = 0;
    char* wordset = NULL;

    init_ncurses();
    
    while(1){
        key  = get_key();
        size = request_wordset(wordset);
    }

    exit_ncurses();

    return(0);
}
