#include <ncurses.h>

int std_x, std_y;

WINDOW* textwin;
int tw_x, tw_y;
int tw_h = 10;

WINDOW* promptwin;
int pw_x, pw_y;
int pw_h = 3;

#define MAXWIDTH 80
#define MINWIDTH 50

void reset_stdscr(){
    clear();
    endwin();
    refresh();
}

void resize_layout(){
    std_x = getmaxx(stdscr);
    std_y = getmaxy(stdscr);
    box(stdscr,0,0);
    mvwprintw(stdscr,1,1,"%dx%d",std_y,std_x);
    refresh();

    int h_offset = ( (std_y)-(tw_h+pw_h) ) / 2;
    /* int h_offset = ( (std_y)-(tw_h+pw_h) ) / 2 ) + 1; //+1 to err up */

    textwin = newwin(tw_h,MINWIDTH,h_offset,(std_x/2)-(MINWIDTH/2));
    box(textwin,0,0);
    mvwprintw(textwin,1,1,"%dx%d",getmaxy(textwin),getmaxx(textwin));
    wrefresh(textwin);

    promptwin = newwin(pw_h,MINWIDTH,(h_offset+tw_h),(std_x/2)-(MINWIDTH/2));
    box(promptwin,0,0);
    mvwprintw(promptwin,1,1,"%dx%d",getmaxy(promptwin),getmaxx(promptwin));
    wrefresh(promptwin);

}

int main(void){
    initscr();
    noecho();
    start_color();

    resize_layout();

    // 27 == ESC
    int c;
    while( (c=getch()) != 27 ){
        switch(c){
            case KEY_RESIZE:
                delwin(textwin);
                delwin(promptwin);
                reset_stdscr();
                resize_layout();
                break;
            default:
                break;
        }
    }
    
    endwin();
    return(0);
}
