#include <stdio.h>
#include <stdlib.h>
/* #include <wchar.h> */

#include "ui.h"

int main(void)
{
    wchar_t key;

    init_ncurses();
    
    while(1){
        key = get_key();
        if( (key = evaluate_key(key)) < 0 ) break;
    }

    exit_ncurses();

    return(0);
}
