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
    int rheight; 
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
    /* nodelay(stdscr,TRUE); */
    start_color();

    box(stdscr,0,0);
    /* refresh(); */

    init_textview(1);
    init_prompt();

    refresh_x3();
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
    
    rc = wget_wch(prompt.window,&key);

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
            redraw_all(textview.rheight);
            break;
    }
}

void redraw_all(int h)
{
    delwin(textview.window);
    delwin(prompt.window);
    reset_ncurses();

    box(stdscr,0,0);
    refresh();

    init_textview(h);
    init_prompt();
}

void init_textview(int newlines)
{
    textview.rheight = newlines;
    textview.height = (newlines + 1) > MAX_HEIGHT ? MAX_HEIGHT : newlines + 1;
    textview.window = newpad(textview.height,WIDTH);
}


void load_wordset_textview(char* wordset, int height)
{
    if(textview.window != NULL)
        delwin(textview.window);

    if(prompt.window != NULL)
        delwin(prompt.window);

    clear();
    box(stdscr,0,0);

    init_textview(height);
    init_prompt();

    write_to_textview(wordset,height);
}

int write_to_textview(char* wordset, int size)
{
    int x,y;
    getyx(prompt.window,y,x); 

    wmove(textview.window,0,0);

    for(int x = 0; x < size; x++){
        waddch(textview.window,*(wordset+x));
    }
    wmove(prompt.window,y,x);

    refresh_x3();
    return(0);
}

void init_prompt(void)
{
    prompt.window = newwin(3,WIDTH,
                           H_OFFSET(textview.height,3)+textview.height,
                           (STD_X/2)-(WIDTH/2));
    keypad(prompt.window,TRUE);
    prompt.posy = 1;
    prompt.posx = 1;
    wmove(prompt.window,prompt.posy,prompt.posx);
    box(prompt.window,0,0);
}

int write_to_prompt(wint_t key)
{
    wchar_t glyph[] = { (wchar_t)key, 0 };
    waddwstr(prompt.window,glyph);
    wrefresh(prompt.window);
}

void refresh_x3(void)
{
    refresh();
/* prefresh(pad, pad_y, pad_x, TOPLEFT_y, TOPLEFT_x, BOTRIGHT_y BOTRIGHT_x) */
    prefresh(textview.window,0,0,
             H_OFFSET(textview.height,3), 
                     ((STD_X/2)-(WIDTH/2))+1,
             H_OFFSET(textview.height,3)+textview.height,
                     ((STD_X/2)-(WIDTH/2))+WIDTH-2 );
    wrefresh(prompt.window);
}
