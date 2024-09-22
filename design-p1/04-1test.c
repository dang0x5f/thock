#include <ncurses.h>

int std_x, std_y;

WINDOW* textwin;
int tw_x, tw_y;
int tw_h = 10;

WINDOW* textwin_scrl;
int tw_offset = 0;

WINDOW* promptwin;
int pw_x, pw_y;
int pw_h = 3;

#define MAXWIDTH 80
#define MINWIDTH 50

int h_offset = 0;

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

    h_offset = ( (std_y)-(tw_h+pw_h) ) / 2;
    /* int h_offset = ( (std_y)-(tw_h+pw_h) ) / 2 ) + 1; //+1 to err up */

    textwin = newpad(200,MINWIDTH);
    for(int x = 0; x < 200; x++)
        wprintw(textwin,"%3d.%s\n",x,"textwintextwin");
    prefresh(textwin,0,0,h_offset,(std_x/2)-(MINWIDTH/2),
             h_offset+tw_h-1,((std_x/2)-(MINWIDTH/2))+MINWIDTH-1);

    promptwin = newwin(pw_h,MINWIDTH,(h_offset+tw_h),(std_x/2)-(MINWIDTH/2));
    box(promptwin,0,0);
    mvwprintw(promptwin,1,1,"%dx%d",getmaxy(promptwin),getmaxx(promptwin));
    wrefresh(promptwin);

}

void scrollup(){
    /* h_offset+tw_h-1: -1 since the line starts at h_offset and counts as 1 row */
    prefresh(textwin,--tw_offset,0,h_offset,(std_x/2)-(MINWIDTH/2),
             h_offset+tw_h-1,((std_x/2)-(MINWIDTH/2))+MINWIDTH-1);
}

void scrolldown(){
    /* h_offset+tw_h-1: -1 since the line starts at h_offset and counts as 1 row */
    prefresh(textwin,++tw_offset,0,h_offset,(std_x/2)-(MINWIDTH/2),
             h_offset+tw_h-1,((std_x/2)-(MINWIDTH/2))+MINWIDTH-1);    
}

int main(void){
    initscr();
    noecho();
    start_color();
    keypad(stdscr,TRUE);

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
            case KEY_UP: 
                if(tw_offset > 0)
                    scrollup();
                break;
            case KEY_DOWN: 
                if(tw_offset < 190)
                    scrolldown();
                break;
            default:
                break;
        }
    }
    
    endwin();
    return(0);
}
