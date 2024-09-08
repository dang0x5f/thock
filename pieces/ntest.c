#include <ncurses.h>
#include <string.h>

int main(void){

    /* WINDOW* win_a; */
    /* WINDOW* win_b; */

    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN); // correct
    init_pair(2, COLOR_BLACK, COLOR_RED);   // wrong
    init_pair(3, COLOR_BLACK, COLOR_WHITE); // current
    /* box(stdscr, 0, 0); */
    refresh();

/*
    win_a = newwin(10,50, 10,15);
    box(win_a, 0, 0);
    win_b = newwin(3,50, 20,15);
    box(win_b, 0, 0);
        wrefresh(win_a);
        wrefresh(win_b);
        mvwgetch(win_b,1,1);
    
    while(true){
        wgetch(win_b);
    }
*/

    char* str = "dang0x5f";
    int len = strlen(str);
    for(int x = 0; x < len; x++){
        if(x < (len - 3))
            addch(COLOR_PAIR(1) | *(str + x));
        else
            addch(COLOR_PAIR(2) | *(str + x));
    }

    refresh();
    getch();

    endwin();

    return(0);
}
