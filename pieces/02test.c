#include <ncurses.h>

int main(void){
    initscr();
    noecho();
    start_color();
    box(stdscr,0,0);
    mvwprintw(stdscr,1,1,"%dx%d",getmaxy(stdscr),getmaxx(stdscr));
    
    refresh();

    int c;
    while( (c = getch()) != 27 ){
        switch(c){
            case KEY_RESIZE:
                clear();
                endwin();
                refresh();
                box(stdscr,0,0);
                mvwprintw(stdscr,1,1,"%dx%d",getmaxy(stdscr),getmaxx(stdscr));
                refresh();
                break;
            default:
                wmove(stdscr,getmaxy(stdscr)-2,1);
                addch(c);
                break;
        }
    }

    endwin();
    return(0);
}
