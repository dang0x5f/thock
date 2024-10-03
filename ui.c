#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <assert.h>

#include "ui.h"
#include "config.h"

#define WIDTH 50
#define MAX_HEIGHT 5

#define STD_X (getmaxx(stdscr))
#define STD_Y (getmaxy(stdscr))
#define H_OFFSET(tv_h,pw_h) ( ((STD_Y)-(tv_h+pw_h)) / 2 )

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

     /* init_pair(1, COLOR_BLUE, COLOR_WHITE); */

    box(stdscr,0,0);
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

wchar_t evaluate_key(wchar_t k)
{
    flushinp();
    switch(k){
        case KEY_RESIZE:
            redraw_all();
            return 0;
            break;
        case KEY_ESC:
            return -1;
            break;
        default:
            return 0;
            break;
    }
}

void redraw_all(void)
{
    delwin(textview.window);
    delwin(prompt.window);
    reset_ncurses();

    box(stdscr,0,0);
    refresh();

    init_textview();
    init_prompt();
}

/* prefresh(window, window_y, window_x,        */                                         
/*          relative_to_stdscr_y_upperleft,    */ 
/*          relative_to_stdscr_x_upperleft,    */     
/*          relative_to_stdscr_y_bottomright,  */ 
/*          relative_to_stdscr_x_bottomright)  */
void init_textview(void)
{
    textview.height = (newlines + 1) > MAX_HEIGHT ? MAX_HEIGHT : newlines + 1;
    textview.window = newpad(textview.height,WIDTH);
      /* wbkgd(textview.window,COLOR_PAIR(1)); // TODO: debug. delete later */ 
    /* write_to_textview_test(); */
    prefresh(textview.window,0,0,
             H_OFFSET(textview.height,3), 
                     ((STD_X/2)-(WIDTH/2))+1,
             H_OFFSET(textview.height,3)+textview.height,
                     ((STD_X/2)-(WIDTH/2))+WIDTH-2 );
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
                           H_OFFSET(textview.height,3)+textview.height,
                           (STD_X/2)-(WIDTH/2));
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

