#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <assert.h>

#include "ui.h"
#include "config.h"

#define WIDTH 50

#define STD_X (getmaxx(stdscr))
#define STD_Y (getmaxy(stdscr))
#define H_OFFSET(tv_h,pw_h) ( ((STD_Y)-(tv_h+pw_h)) / 2 )

/* TODO maybe stdscr struct */

/* typedef struct { */
/*     WINDOW* window; */
/*     int posx; */
/*     int posy; */
/*     int height; */
/* }TextView_Scroll; */

/* static TextView_Scroll textview_scroll; */

/* void init_textview_scroll(void){ */
/*     /1* TODO *1/ */
/* } */

/* void draw_textview_scroll(void){ */
/*     /1* TODO *1/ */
/* } */

typedef struct {
    WINDOW* window;
    int posx;
    int posy;
    int height;
    int offset;
} TextView;

typedef struct {
    WINDOW* window;
    int posx;
    int posy;
    int height;
} Prompt;

/* Globals */
static TextView textview;
static Prompt   prompt;

void init_ncurses(void)
{
    assert(initscr() != NULL);

    noecho();
    set_escdelay(25);
    keypad(stdscr,TRUE);
    start_color();
    refresh();

    init_textview();
    init_prompt();

    /* box(stdscr,0,0); */
    /* refresh(); */
}

void reset_ncurses(void)
{
    clear();
    endwin();
    refresh();
}

void exit_ncurses(void)
{
    endwin();
}

wchar_t get_key(void)
{
    return getch();
}

void init_textview(void)
{
    textview.window = newpad(nls+1,WIDTH);
    write_to_textview_test();
    prefresh(textview.window,0,0,
             H_OFFSET(nls+1,3),     (STD_X/2)-(WIDTH/2)+1,
             H_OFFSET(nls+1,3)+nls, ((STD_X/2)-(WIDTH/2))+WIDTH-1 );
}

/* TODO remove this later */
void write_to_textview_test()
{
    mvwprintw(textview.window,0,0,"%s",sample_text);
}

void draw_textview(void)
{
    /* TODO */
}

void write_to_textview(char* text, char s[], int sl)
{
    /* TODO */
}

void init_prompt(void)
{
    prompt.window = newwin(3,WIDTH,
                           H_OFFSET(nls+1,3)+nls+1,(STD_X/2)-(WIDTH/2));
    box(prompt.window,0,0);
    wrefresh(prompt.window);
}

void draw_prompt(void)
{
    /* TODO */
}

void write_to_prompt(int c, char b[], int bl)
{
    /* TODO */
}
