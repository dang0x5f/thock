#include <stdio.h>
#include <stdlib.h>
/* #include <wchar.h> */

#include <string.h>

#include "dictmod.h"
#include "thock.h"
#include "ui.h"

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

    /* TODO: */
    /* newlines = size / width */
    /* format temp_wordset -> wordset, free temp_wordset */

    return size;
}

int main(void)
{
    char* words = NULL;
    int size = 0;

    words = get_random_wordset(&size);

    printf("%s\n",words);
printf("%d , %d\n", size, (int)strlen(words));

    free(words);

    words = get_random_wordset(&size);

    printf("%s\n",words);
printf("%d , %d\n", size, (int)strlen(words));

    free(words);
    return(0);    
}

int main2(void)
{
    int size;
    wchar_t key;
    char* wordset;

    init_ncurses();
    size = request_wordset(wordset);
    
    while(1){
        key = get_key();
        if( (key = evaluate_key(key)) < 0 ) break;
    }

    exit_ncurses();

    return(0);
}
