#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <assert.h>

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
    /* TODO */
}

void draw_textview(void)
{
    /* TODO */
}

void init_prompt(void)
{
    /* TODO */
}

void draw_prompt(void)
{
    /* TODO */
}
