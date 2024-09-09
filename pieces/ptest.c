#include <ncurses.h>
#include <string.h>

int main(void){

    initscr();

    refresh();
    getch();

    endwin();
    return(0);
}
