#include <stdio.h>
#include <stdlib.h>
/* #include <wchar.h> */

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

int main(void)
{
    wchar_t key;

    init_ncurses();
    
    while(1){
        key = get_key();
        if( (key = evaluate_key(key)) < 0 ) break;
    }

    exit_ncurses();

    /* TODO: delete later */
    printf("%s\n", dict.get_wordset(20));

    return(0);
}
