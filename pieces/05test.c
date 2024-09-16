#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

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

char* dyn_text;
int nls;

int h_offset = 0;

char* dynamic_text_gen(){
    // gen nl count
    // create pointer
    // write lines
    // return ptr

    srand(time(NULL));
    nls = (rand()%40)+1;
    
    char* text = calloc(nls*2+1,sizeof(char));

    for(int x = 0; x < nls*2; x += 2)
        text[x] = '0' + x;
    for(int x = 1; x < nls*2; x += 2)
        text[x] = '\n';
    text[nls*2] = '\0';

    return(text);
}

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

    /* int h_offset = ( (std_y)-(tw_h+pw_h) ) / 2 ) + 1; //+1 to err up */

    if(dyn_text != NULL)
        free(dyn_text);
    dyn_text = dynamic_text_gen();
    textwin = newpad(nls,MINWIDTH);
    wprintw(textwin,"%s",dyn_text);
    tw_h = (nls > 10) ? 10 : nls;
    h_offset = ( (std_y)-(tw_h+pw_h) ) / 2;
    /* for(int x = 0; x < 200; x++) */
    /*     wprintw(textwin,"%3d.%s\n",x,"textwintextwin"); */
    prefresh(textwin,0,0,h_offset,(std_x/2)-(MINWIDTH/2),
             h_offset+tw_h-1,((std_x/2)-(MINWIDTH/2))+MINWIDTH-1);

    promptwin = newwin(pw_h,MINWIDTH,(h_offset+tw_h),(std_x/2)-(MINWIDTH/2));
    box(promptwin,0,0);
    mvwprintw(promptwin,1,1,"%dx%d",getmaxy(promptwin),getmaxx(promptwin));
    wrefresh(promptwin);

    /* mvwprintw(stdscr,std_y-1,1,"%d",nls); */
    /* refresh(); */
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
    
                delwin(textwin);
                delwin(promptwin);
    endwin();
    return(0);
}
