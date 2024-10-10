#define _XOPEN_SOURCE_EXTENDED 1
#include <ncurses.h>
#include <assert.h>
#include <locale.h>

#include "ui.h"
#include "util.h"
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
    int wordset_len;
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

    init_textview(0);
    init_prompt_window();

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

int get_key(uint8_t* wordset_state)
{
    int rc;
    wint_t key;
    
    rc = wget_wch(prompt.window,&key);

    if(key == ctrl('x')){
        exit_ncurses();
        exit(0);
    }

    // TODO: determine fate of function keys
    if(key == KEY_ESC)
        flushinp(); // eats remaining function characters

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
            redraw_all();
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

void redraw_all(void)
{
    delwin(textview.window);
    delwin(prompt.window);
    reset_ncurses();

    box(stdscr,0,0);

    reinit_textview();
    reinit_prompt_window();

    restore_wordset_buffer_data();
}

void init_textview(int rows)
{
    textview.rheight = rows;
    // +1 row index starts at 0
    textview.height = (rows+1) > MAX_HEIGHT ? MAX_HEIGHT : rows+1; 
    textview.window = newpad(textview.height,WIDTH);
}

void reinit_textview(void)
{
    textview.window = newpad(textview.height,WIDTH);
}

void reinit_prompt_window(void)
{
    prompt.window = newwin(3,WIDTH,
                           h_offset(textview.height,3)+textview.height,
                           (std_x/2)-(WIDTH/2));
    keypad(prompt.window,TRUE);
    box(prompt.window,0,0);
}

void load_wordset_textview(wchar_t* wordset, int wordset_len, uint8_t* wordset_state)
{
    if(textview.window != NULL)
        delwin(textview.window);

    if(prompt.window != NULL){
        if(prompt.buffer) free(prompt.buffer);
        delwin(prompt.window);
    }

    clear();
    box(stdscr,0,0);
    init_textview(wordset_len/WIDTH);
    init_prompt_window();

    if(textview.wordset) free(textview.wordset);
    textview.wordset_len = wcslen(wordset);
    textview.wordset = malloc((textview.wordset_len+1) * sizeof(wchar_t));
    wcscpy(textview.wordset,wordset);

    init_wordset_state(wordset,wordset_state,textview.wordset_len);

    write_to_textview(textview.wordset,wordset_state);
}

int write_to_textview(wchar_t* wordset, uint8_t* wordset_state)
{
    int x,y;
    getyx(prompt.window,y,x); 

    wmove(textview.window,0,0);

    for(int x = 0; x < textview.wordset_len; x++){
        /* TODO: this needs to output wchar */
        /* waddwstr(textview.window,(wordset+x)); */
        if( *(wordset_state+x) == CURSOR)
            waddch(textview.window, WA_REVERSE | *(wordset+x));
        else
            waddch(textview.window, *(wordset+x));

    }
    wmove(prompt.window,y,x);

    refresh_x3();

    /* TODO: return something meaningful */
    return(0);
}

void init_prompt_window(void)
{
    prompt.window = newwin(3,WIDTH,
                           h_offset(textview.height,3)+textview.height,
                           (std_x/2)-(WIDTH/2));
    keypad(prompt.window,TRUE);
    box(prompt.window,0,0);

    init_prompt_attributes();

    wmove(prompt.window,prompt.posy,prompt.posx);
}

void init_prompt_attributes(void)
{
    prompt.buffer = malloc( (buf_len+1)*sizeof(wchar_t) );
    prompt.buf_pos = 0;

    prompt.posy = 1;
    prompt.posx = 1;
}

int write_to_prompt(wint_t key)
{
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

void restore_wordset_buffer_data(void)
{
    for(int x = 0; x < textview.wordset_len; x++){
        waddch(textview.window,*(textview.wordset+x));
    }
    
    mvwaddwstr(prompt.window, 1,1, prompt.buffer);

    refresh_x3();
}
