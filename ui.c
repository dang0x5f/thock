#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <ncurses.h>
#include <assert.h>

#include "ui.h"
#include "util.h"
#include "config.h"

#define WIDTH 50
#define MAX_HEIGHT 5

#define buf_len    ( WIDTH - 2 )

#define STD_X      ( getmaxx(stdscr) )
#define STD_Y      ( getmaxy(stdscr) )
#define OFFSET_X   ( (STD_X / 2) - (WIDTH / 2) )
#define OFFSET_Y   ( (STD_Y - (textview.rows + prompt.rows) ) / 2 )


typedef struct {
    uint8_t  rows;
    uint8_t  cols;
    uint32_t total_rows;       
    uint32_t offset;
    WINDOW*  win;
} TextView;

typedef struct {
    wchar_t* text;
    uint8_t* text_state;
    uint32_t text_length;
} Wordset;

typedef struct {
    uint8_t  cols;
    uint8_t  rows;
    uint8_t  xpos;
    uint8_t  ypos;
    uint8_t  buffer_index;
    uint8_t  buffer_length;
    wchar_t* buffer;
    WINDOW*  win;
} Prompt;


/* Globals */
static TextView textview;
static Wordset  wordset;
static Prompt   prompt;


bool initialize_stdscr(void)
{
    if(!initscr())
        return(false);

    /* TODO: nodelay awkwardly prints out shell prompt... */
    /* nodelay(stdscr,TRUE); */
    noecho();
    set_escdelay(25);
    start_color();
    toggle_cursor(CVS_INVISIBLE);
    box(stdscr,0,0);

    refresh();
    
    return(true);
}

bool initialize_wordset(void)
{
    wordset.text        = NULL;
    wordset.text_state  = NULL;
    wordset.text_length = 0;

    /* TODO: explicitly declare modules[0] ? */
    wordset.text = modules[0].get_wordset(&wordset.text_length);
    if( (wordset.text == NULL) || (wordset.text_length == 0) ){
        endwin();
        return(false);
    }

    wordset.text_state = malloc( (wordset.text_length+1) * sizeof(uint8_t) );
    if ( (wordset.text_state == NULL) || !initialize_wordset_state() ){
        endwin();
        return(false);
    }

    return(true);
}

bool initialize_wordset_state(void)
{
    uint32_t index = 0;
    wchar_t* iter = wordset.text;

    while(index < wordset.text_length){
        if(index == 0)
            *(wordset.text_state+index) = WC_CURSOR;
        /* TODO: add macros for space keys */
        else if(*iter == '\040')
            *(wordset.text_state+index) = WC_CHECKPOINT_ON;
        else
            *(wordset.text_state+index) = WC_OUT_OF_REACH;

        index++;
    }

    return(true);
}

bool initialize_textview(void)
{
    /* TODO: WIDTH might need to be buf_len, ie WIDTH-2 */ 
    textview.total_rows = wordset.text_length/WIDTH;
    if(wordset.text_length%WIDTH != 0)
        textview.total_rows += 1;
    
    textview.cols   = WIDTH;
    /* TODO: +1 row index starts at 0 */
    textview.rows   = (textview.total_rows) > MAX_HEIGHT ? MAX_HEIGHT : textview.total_rows;
    textview.offset = 0;

    if( (textview.total_rows < 1) || (textview.cols < 1) ){
        endwin();
        fprintf(stderr, "  wordsetlen: %d\n  totalrow: %d\n  cols: %d\n  rows: %d\n", 
                wordset.text_length, textview.total_rows, textview.cols, textview.rows);
        return(false);
    }

    textview.win = newpad(textview.total_rows, WIDTH);

    if(textview.win == NULL){
        endwin();
        fprintf(stderr, "textview.win null\n");
        return(false);
    }


    return(true);
}

bool initialize_prompt(void)
{
    prompt.cols = WIDTH;
    prompt.rows = 3;
    prompt.xpos = 1;
    prompt.ypos = 1;

    prompt.buffer_index = 0;
    prompt.buffer_length = buf_len;
    prompt.buffer = malloc( (prompt.buffer_length+1) * sizeof(wchar_t) );
    if(!prompt.buffer){
        endwin();
        fprintf(stderr,"prompt.buffer null");
        return(false);
    }

    prompt.win = newwin(prompt.rows, prompt.cols,
                        OFFSET_Y+textview.rows, OFFSET_X);
    if(!prompt.win){
        endwin();
        fprintf(stderr,"prompt.win null");
        return(false);
    }

    keypad(prompt.win,TRUE);
    box(prompt.win,0,0);

    /* TODO: when to set this? in draw? */
    /* wmove(prompt.window,prompt.posy,prompt.posx); */

    return(true);
}

void draw_stdscr(void)
{
    wrefresh(stdscr);
}

void draw_textview(void)
{
    draw_textview_wordset();
    /* prefresh(WINDOW* pad, int pad_y, int pad_x, int topleft_y, 
     *          int topleft_x, int botright_y, int botright_x) */
    prefresh(textview.win , textview.offset , 0 , 
             OFFSET_Y , OFFSET_X + 1 ,
             OFFSET_Y + textview.rows , OFFSET_X + (WIDTH-2) );

}

void draw_textview_wordset(void)
{
    /* TODO: this needs to output wchar */
    waddwstr(textview.win, wordset.text);
    /* for(int x = 0; x < wordset.text_length; x++){ */
        /* if( *(wordset.text_state+x) == WC_CURSOR) */
        /*     waddch(textview.win, WA_REVERSE | *(wordset.text+x)); */
        /* else */
        /*     waddch(textview.win, *(wordset.text+x)); */
}

void draw_prompt(void)
{
    wrefresh(prompt.win);
}

wint_t get_keycode(void)
{
    wint_t key;
    if(wget_wch(prompt.win,&key)==ERR)
        return(WEOF);

    switch(key){
        case KEY_RESIZE:
            do_resize();
            key = WEOF;
            break;
        case KEY_ESCAPE:
            flushinp();
            key = WEOF;
            break;
        case KEY_BACKSPACE:
            backspace_buffer();
            key = WEOF;
            break;
    }
    return(key);
}

bool use_keycode(wint_t key)
{
    if(key < 32) return(true);
    update_buffer(key);
    write_prompt();

    /* update_state(); */
    /* write_wordset(); */

    return(true);
}

/* ... [48] [49] [50] */

void backspace_buffer(void)
{
    if(prompt.buffer_index > 0){
        prompt.buffer_index -= 1;
        mvwaddwstr(prompt.win,1,prompt.buffer_index+1,L" ");
        *(prompt.buffer+prompt.buffer_index) = (wchar_t)'\000';

        write_prompt();
        /* update_state(); */
        /* write_wordset(); */
    }
}

void update_buffer(wint_t key)
{
    if(prompt.buffer_index == prompt.buffer_length){
        *(prompt.buffer+prompt.buffer_index-1) = key;
        *(prompt.buffer+prompt.buffer_length)  = (wchar_t)'\000';
    }else{
        *(prompt.buffer+prompt.buffer_index) = key;
        prompt.buffer_index += 1;
        *(prompt.buffer+prompt.buffer_index) = (wchar_t)'\000';
    }
}

void write_prompt(void)
{
    mvwaddwstr(prompt.win,1,1,prompt.buffer);
}

/* TODO: clean this up a bit */
void do_resize(void)
{
    free_textview();
    free_prompt();
    free_stdscr_resize();

    if(too_small()){
        mvprintw(1,1,"too smol...");
        draw_stdscr();
    }else{
        box(stdscr,0,0);

        textview.win = newpad(textview.total_rows, WIDTH);
        if(textview.win == NULL){
            endwin();
            fprintf(stderr, "textview.win null\n");
            /* return(false); */
        }

        prompt.win = newwin(prompt.rows, prompt.cols,
                            OFFSET_Y + textview.rows, OFFSET_X);
        if(!prompt.win){
            endwin();
            fprintf(stderr,"prompt.win null");
            /* return(false); */
        }
        keypad(prompt.win,TRUE);
        box(prompt.win,0,0);

        draw_stdscr();
        draw_textview();
        draw_prompt();
        write_prompt();
    }
}

bool too_small(void)
{
    if( STD_X < (WIDTH+4) || STD_Y < ((textview.rows + prompt.rows)+4) )
        return(true);
    else
        return(false);
}

void place_cursor(void)
{
    wmove(prompt.win,1,1+prompt.buffer_index);
}

/* TODO: add global current cursor. assign it here */
void toggle_cursor(CursorVisibilityState cvs)
{
    curs_set(cvs);
}

void free_stdscr_resize(void)
{
    endwin();
    refresh();
    clear();
}

void free_stdscr_exit(void)
{
    endwin();
    clear();
}

void free_wordset(void)
{
    free(wordset.text_state);
    free(wordset.text);
}

void free_textview(void)
{
    delwin(textview.win);
}

void free_buffer(void)
{
    free(prompt.buffer);
}

void free_prompt(void)
{
    delwin(prompt.win);
}

// ---------------------------------------------------------------

/* void init_ncurses(void) */
/* { */
/*     setlocale(LC_ALL,""); */
/*     assert(initscr() != NULL); */

/*     noecho(); */
/*     set_escdelay(25); */
/*     nodelay(stdscr,TRUE); */
/*     start_color(); */

/*     box(stdscr,0,0); */

/*     init_textview(0); */
/*     init_prompt_window(); */

/*     refresh_x3(); */
/* } */

/* void reset_ncurses(void) */
/* { */
/*     clear(); */
/*     endwin(); */
/*     refresh(); */
/* } */

/* void exit_ncurses(void) */
/* { */
/*     delwin(textview.window); */

/*     if(prompt.buffer) free(prompt.buffer); */
/*     delwin(prompt.window); */

/*     endwin(); */
/* } */

/* int get_key(uint8_t* wordset_state) */
/* { */
/*     int rc; */
/*     wint_t key; */
    
/*     rc = wget_wch(prompt.window,&key); */

/*     if(key == ctrl('x')){ */
/*         exit_ncurses(); */
/*         exit(0); */
/*     } */

    /* if(key == KEY_ESC) */
        /* flushinp(); // eats remaining function characters */

    /* switch(rc){ */
    /*     case OK: */
    /*         if(key >= 32) write_to_prompt(key); */
    /*         break; */
    /*     case KEY_CODE_YES: */
    /*         evaluate_key(key); */
    /*         break; */
    /*     case ERR: */
    /*         break; */
    /* } */

    /* return(0); */
/* } */

/* void evaluate_key(wint_t key) */
/* { */
/*     switch(key){ */
/*         case KEY_RESIZE: */
/*             redraw_all(); */
/*             break; */
/*         case KEY_BACKSPACE: */
/*             if(prompt.buf_pos > 0){ */
/*                 prompt.buf_pos--; */
/*                 *(prompt.buffer+prompt.buf_pos) = (wchar_t)'\0'; */
/*                 wclear(prompt.window); */
/*                 box(prompt.window,0,0); */
/*                 mvwaddwstr(prompt.window, 1,1, prompt.buffer); */
/*                 wrefresh(prompt.window); */
/*             } */
/*             break; */
/*     } */
/* } */

/* void redraw_all(void) */
/* { */
/*     delwin(textview.window); */
/*     delwin(prompt.window); */
/*     reset_ncurses(); */

/*     box(stdscr,0,0); */

/*     reinit_textview(); */
/*     reinit_prompt_window(); */

/*     restore_wordset_buffer_data(); */
/* } */

/* void init_textview(int rows) */
/* { */
/*     textview.rheight = rows; */
/*     // +1 row index starts at 0 */
/*     textview.height = (rows+1) > MAX_HEIGHT ? MAX_HEIGHT : rows+1; */ 
/*     textview.window = newpad(textview.height,WIDTH); */
/* } */

/* void reinit_textview(void) */
/* { */
/*     textview.window = newpad(textview.height,WIDTH); */
/* } */

/* void reinit_prompt_window(void) */
/* { */
/*     prompt.window = newwin(3,WIDTH, */
/*                            h_offset(textview.height,3)+textview.height, */
/*                            (std_x/2)-(WIDTH/2)); */
/*     keypad(prompt.window,TRUE); */
/*     box(prompt.window,0,0); */
/* } */

/* void load_wordset_textview(wchar_t* wordset, int wordset_len, uint8_t* wordset_state) */
/* { */
/*     if(textview.window != NULL) */
/*         delwin(textview.window); */

/*     if(prompt.window != NULL){ */
/*         if(prompt.buffer) free(prompt.buffer); */
/*         delwin(prompt.window); */
/*     } */

/*     clear(); */
/*     box(stdscr,0,0); */
/*     init_textview(wordset_len/WIDTH); */
/*     init_prompt_window(); */

/*     if(textview.wordset) free(textview.wordset); */
/*     textview.wordset_len = wcslen(wordset); */
/*     textview.wordset = malloc((textview.wordset_len+1) * sizeof(wchar_t)); */
/*     wcscpy(textview.wordset,wordset); */

/*     init_wordset_state(wordset,wordset_state,textview.wordset_len); */

/*     write_to_textview(textview.wordset,wordset_state); */
/* } */

/* int write_to_textview(wchar_t* wordset, uint8_t* wordset_state) */
/* { */
/*     int x,y; */
/*     getyx(prompt.window,y,x); */ 

/*     wmove(textview.window,0,0); */

/*     for(int x = 0; x < textview.wordset_len; x++){ */
/*         /1* waddwstr(textview.window,(wordset+x)); *1/ */
/*         if( *(wordset_state+x) == CURSOR) */
/*             waddch(textview.window, WA_REVERSE | *(wordset+x)); */
/*         else */
/*             waddch(textview.window, *(wordset+x)); */

/*     } */
/*     wmove(prompt.window,y,x); */

/*     refresh_x3(); */

/*     return(0); */
/* } */

/* void init_prompt_window(void) */
/* { */
/*     prompt.window = newwin(3,WIDTH, */
/*                            h_offset(textview.height,3)+textview.height, */
/*                            (std_x/2)-(WIDTH/2)); */
/*     keypad(prompt.window,TRUE); */
/*     box(prompt.window,0,0); */

/*     init_prompt_attributes(); */

/*     wmove(prompt.window,prompt.posy,prompt.posx); */
/* } */

/* void init_prompt_attributes(void) */
/* { */
/*     prompt.buffer = malloc( (buf_len+1)*sizeof(wchar_t) ); */
/*     prompt.buf_pos = 0; */

/*     prompt.posy = 1; */
/*     prompt.posx = 1; */
/* } */

/* int write_to_prompt(wint_t key) */
/* { */
/*     if(prompt.buf_pos == buf_len-1){ */
/*         *(prompt.buffer + prompt.buf_pos) = key; */
/*         *(prompt.buffer + buf_len) = (wchar_t)'\0'; */
/*     }else{ */
/*         *(prompt.buffer + prompt.buf_pos) = key; */
/*         prompt.buf_pos++; */
/*         *(prompt.buffer + prompt.buf_pos) = (wchar_t)'\0'; */
/*     } */
    
/*     mvwaddwstr(prompt.window, 1,1, prompt.buffer); */
/*     wrefresh(prompt.window); */

/*     return(0); */
/* } */

/* void refresh_x3(void) */
/* { */
/*     refresh(); */
/* /1* prefresh(pad, pad_y, pad_x, TOPLEFT_y, TOPLEFT_x, BOTRIGHT_y BOTRIGHT_x) *1/ */
/*     prefresh(textview.window,0,0, */
/*              h_offset(textview.height,3), */ 
/*                      ((std_x/2)-(WIDTH/2))+1, */
/*              h_offset(textview.height,3)+textview.height, */
/*                      ((std_x/2)-(WIDTH/2))+WIDTH-2 ); */
/*     wrefresh(prompt.window); */
/* } */

/* void restore_wordset_buffer_data(void) */
/* { */
/*     for(int x = 0; x < textview.wordset_len; x++){ */
/*         waddch(textview.window,*(textview.wordset+x)); */
/*     } */
    
/*     mvwaddwstr(prompt.window, 1,1, prompt.buffer); */

/*     refresh_x3(); */
/* } */
