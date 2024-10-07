#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <assert.h>
#include <locale.h>

#include "ui.h"
#include "config.h"

#define WIDTH 50
#define MAX_HEIGHT 5

#define CTRL(x) ((x) & 0x1F)
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
    setlocale(LC_ALL,"");
    assert(initscr() != NULL);

    noecho();
    set_escdelay(25);
    nodelay(stdscr,TRUE);
    keypad(stdscr,TRUE);
    start_color();

    box(stdscr,0,0);
    refresh();

    init_textview();
    init_prompt();
}

void reset_ncurses(void)
{
    clear();
    endwin();
    refresh();
}

void exit_ncurses(void)
{
    delwin(textview.window);
    delwin(prompt.window);
    endwin();
}

int get_key(void)
{
    int rc;
    wint_t key;
    
    rc = get_wch(&key);

    if(key == CTRL('x')){
        exit_ncurses();
        exit(0);
    }

    // TODO: determine fate of function keys
    if(key == '\033'){            
        while(rc != ERR) rc = get_wch(&key);
    }

    switch(rc){
        case OK:
            if(key >= 32) write_to_prompt(key);
            break;
        case KEY_CODE_YES:
            evaluate_key(key);
            break;
        case ERR:
            break;
    }

/* if((wint_t)key == '\033'){ */
/*     fprintf(stderr,"esc"); */
/*     exit(-669); */
/* } */
}

void evaluate_key(wint_t key)
{
    switch(key){
        case KEY_RESIZE:
            redraw_all();
            break;
        case KEY_F(12):
            exit_ncurses();
            exit(-1);
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
    prefresh(textview.window,0,0,
             H_OFFSET(textview.height,3), 
                     ((STD_X/2)-(WIDTH/2))+1,
             H_OFFSET(textview.height,3)+textview.height,
                     ((STD_X/2)-(WIDTH/2))+WIDTH-2 );
}


void draw_textview(void)
{
}

int write_to_textview(char* text)
{
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
}

int write_to_prompt(wint_t key)
{
    wchar_t glyph[] = { (wchar_t)key, 0 };
    waddwstr(prompt.window,glyph);
    wrefresh(prompt.window);
}


