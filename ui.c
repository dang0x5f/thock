#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <assert.h>
#include <locale.h>

#include "ui.h"
#include "config.h"

#define WIDTH 50
#define MAX_HEIGHT 5

#define std_x               ( getmaxx(stdscr) )
#define std_y               ( getmaxy(stdscr) )
#define buf_len             ( WIDTH - 2 )

#define ctrl(x)             ( (x) & 0x1F )
#define h_offset(tv_h,pw_h) ( ((std_y)-(tv_h+pw_h)) / 2 )

/* TODO: rename attributes */
typedef struct {
    WINDOW* window;
    int posx;
    int posy;
    int height;   // visible height
    int rheight;  // real height
    int offset;
    wchar_t* wordset;
} TextView;

typedef struct {
    WINDOW* window;
    int posx;
    int posy;
    int height;
    wchar_t* buffer;
    uint8_t  buf_pos;
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
    start_color();

    box(stdscr,0,0);

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

    if(prompt.buffer) free(prompt.buffer);
    delwin(prompt.window);

    endwin();
}

int get_key(void)
{
    int rc;
    wint_t key;
    
    rc = wget_wch(prompt.window,&key);

    if(key == ctrl('x')){
        exit_ncurses();
        exit(0);
    }

    // TODO: determine fate of function keys
    if(key == '\033'){            
        flushinp(); // flush eats remaining function characters
        /* while(rc != ERR){ */
        /*     rc = wget_wch(prompt.window,&key); */
        /* } */
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

    /* TODO: return something meaningful */
    return(0);
}

void evaluate_key(wint_t key)
{
    switch(key){
        case KEY_RESIZE:
            redraw_all(textview.rheight);
            break;
        case KEY_BACKSPACE:
            if(prompt.buf_pos > 0){
                prompt.buf_pos--;
                *(prompt.buffer+prompt.buf_pos) = (wchar_t)'\0';
                wclear(prompt.window);
                box(prompt.window,0,0);
                mvwaddwstr(prompt.window, 1,1, prompt.buffer);
                wrefresh(prompt.window);
            }
            break;
    }
}

void redraw_all(int h)
{
    delwin(textview.window);
    /* if(prompt.buffer) free(prompt.buffer); */
    delwin(prompt.window);
    reset_ncurses();

    box(stdscr,0,0);
    refresh();

    init_textview(h);
    init_prompt();

    write_to_textview(textview.wordset,wcslen(textview.wordset));
}

void init_textview(int newlines)
{
    textview.rheight = newlines;
    textview.height = (newlines + 1) > MAX_HEIGHT ? MAX_HEIGHT : newlines + 1;
    textview.window = newpad(textview.height,WIDTH);
}


void load_wordset_textview(wchar_t* wordset, int height)
{
    if(textview.window != NULL)
        delwin(textview.window);

    if(prompt.window != NULL){
        if(prompt.buffer) free(prompt.buffer);
        delwin(prompt.window);
    }

    clear();
    box(stdscr,0,0);

    init_textview(height);
    init_prompt();

    if(textview.wordset) free(textview.wordset);
    textview.wordset = malloc((wcslen(wordset)+1) * sizeof(wchar_t));
    wcscpy(textview.wordset,wordset);
    
    write_to_textview(textview.wordset,height);
}

int write_to_textview(wchar_t* wordset, int size)
{
    int x,y;
    getyx(prompt.window,y,x); 

    wmove(textview.window,0,0);

    for(int x = 0; x < size; x++){
        waddch(textview.window,*(wordset+x));
    }
    wmove(prompt.window,y,x);

    refresh_x3();

    /* TODO: return something meaningful */
    return(0);
}

void init_prompt(void)
{
    prompt.window = newwin(3,WIDTH,
                           h_offset(textview.height,3)+textview.height,
                           (std_x/2)-(WIDTH/2));
    keypad(prompt.window,TRUE);
    prompt.posy = 1;
    prompt.posx = 1;
    prompt.buffer = malloc( (buf_len+1)*sizeof(wchar_t) );
    prompt.buf_pos = 0;
    wmove(prompt.window,prompt.posy,prompt.posx);
    box(prompt.window,0,0);
}

int write_to_prompt(wint_t key)
{
    /* wchar_t glyph[] = { (wchar_t)key, 0 }; */

    if(prompt.buf_pos == buf_len-1){
        *(prompt.buffer + prompt.buf_pos) = key;
        *(prompt.buffer + buf_len) = (wchar_t)'\0';
    }else{
        *(prompt.buffer + prompt.buf_pos) = key;
        prompt.buf_pos++;
        *(prompt.buffer + prompt.buf_pos) = (wchar_t)'\0';
    }
    
    mvwaddwstr(prompt.window, 1,1, prompt.buffer);
    wrefresh(prompt.window);

    /* TODO: return something meaningful */
    return(0);
}

void refresh_x3(void)
{
    refresh();
/* prefresh(pad, pad_y, pad_x, TOPLEFT_y, TOPLEFT_x, BOTRIGHT_y BOTRIGHT_x) */
    prefresh(textview.window,0,0,
             h_offset(textview.height,3), 
                     ((std_x/2)-(WIDTH/2))+1,
             h_offset(textview.height,3)+textview.height,
                     ((std_x/2)-(WIDTH/2))+WIDTH-2 );
    wrefresh(prompt.window);
}
