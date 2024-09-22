#include <stdio.h>
#include <wchar.h>

#include "ui.h"

int main(void){

    init_ncurses();
    
    wchar_t key;
    while( (key=get_key()) != KEY_ESC){
        
    }

    exit_ncurses();

    return(0);
}
