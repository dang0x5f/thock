#include <ncurses.h>

WINDOW* textwin;
int tw_x, tw_y;

WINDOW* promptwin;
int pw_x, pw_y;

#define MAXWIDTH 80
#define MINWIDTH 50

int main(void){
    initscr();
    noecho();
    start_color();
    box(stdscr,0,0);
    mvwprintw(stdscr,1,1,"%dx%d",getmaxy(stdscr),getmaxx(stdscr));
    refresh();

    int h = ((getmaxy(stdscr)-13)/2)+1;

    textwin = newwin(10,MINWIDTH,h,(getmaxx(stdscr)/2)-25);
    box(textwin,0,0);
    mvwprintw(textwin,1,1,"%dx%d",getmaxy(textwin),getmaxx(textwin));
    wrefresh(textwin);

    promptwin = newwin(3,MINWIDTH,h+10,(getmaxx(stdscr)/2)-25);
    box(promptwin,0,0);
    mvwprintw(promptwin,1,1,"%dx%d",getmaxy(promptwin),getmaxx(promptwin));
    wrefresh(promptwin);


    getch();
    
    endwin();
    return(0);
}
