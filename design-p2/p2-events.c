#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <locale.h>

#define KEY_ESCAPE 27
int running = 1;

WINDOW* one;
/* WINDOW* two; */

void keyup(WINDOW*);
void keydown(WINDOW*);

void (*keypress_handler[KEY_MAX])(WINDOW*) = {
    [KEY_UP]   = keyup,
    [KEY_DOWN] = keydown,
};

void keyup(WINDOW* win){
    wclear(win);
    box(one,0,0);
    mvwprintw(win,1,1,"%s", "keyup");
    wrefresh(win);
}

void keydown(WINDOW* win){
    wclear(win);
    box(one,0,0);
    mvwprintw(win,1,1,"%s", "keydown");
    wrefresh(win);
}

int main(void){
    initscr();
    noecho();
    keypad(stdscr,TRUE);
    start_color();
    refresh();
    
    one = newwin(20,10, 2,2);
    box(one,0,0);
    wrefresh(one);
    
    /* two = newwin(10,10, 20,20); */
    /* box(two,0,0); */

    setlocale(LC_ALL, "");
    cchar_t heart = {
        A_NORMAL,
        L"\u2665"
    };

    wchar_t c;
    while(running){
        c = getch();
        (keypress_handler[c])(one);

        add_wch(&heart);
        refresh();
        c = getch();
        heart.chars[0] = c;
    }

    endwin();
    return(0);
}
