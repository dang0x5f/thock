#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int size;
    char* temp_wordset;
    if(wordset != NULL) free(wordset);
    
    /* temp_wordset = data_modules[0].get_wordset(&size); */

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
    /* printf("%s\n",words); */
    /* printf("%d , %d\n", size, (int)strlen(words)); */

    if(words != NULL) free(words);

    return(0);    
}

int main(void)
{
    int size;
    wchar_t key;
    char* wordset;

    init_ncurses();
    size = request_wordset(wordset);
    
    while(1){
        key = get_key();
    }

    exit_ncurses();

    return(0);
}
