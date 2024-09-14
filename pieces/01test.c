#include <ncurses.h>
#include <signal.h>

void sig_handler(int);

void sig_handler(int sig){
    int x,y;
    switch(sig){
        case SIGWINCH:
            endwin();
            refresh();

            clear();
            box(stdscr,0,0);
            getmaxyx(stdscr,y,x);
            mvwprintw(stdscr,1,1,"%dx%d",y,x);

            refresh();
            break;
    }
}

int main(void){
    int x,y;

    initscr();
    start_color();
    signal(SIGWINCH, sig_handler);

    box(stdscr,0,0);
    getmaxyx(stdscr,y,x);
    mvwprintw(stdscr,1,1,"%dx%d",y,x);
    
    refresh();

    while(1){
        getch();
    }

    endwin();

    return(0);
}
