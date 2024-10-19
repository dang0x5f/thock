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
    uint8_t  seg_start;
    uint8_t  seg_end;
    wchar_t* wctext;
    cchar_t* wcextended;
    uint8_t  wctext_cursor;
    uint8_t* wctext_state;
    uint32_t wctext_length;
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


    /* TODO: init color pairs separately */
    init_pair(1,COLOR_BLACK,COLOR_GREEN);
    init_pair(2,COLOR_BLACK,COLOR_RED);

    refresh();
    
    return(true);
}

bool initialize_wordset(void)
{
    wordset.seg_start     = 0;
    wordset.seg_end       = 0;
    wordset.wctext        = NULL;
    wordset.wcextended    = NULL;
    wordset.wctext_state  = NULL;
    wordset.wctext_length = 0;

    /* TODO: explicitly declare modules[0] ? */
    wordset.wctext = modules[0].get_wordset(&wordset.wctext_length);
    if( (wordset.wctext == NULL) || (wordset.wctext_length == 0) ){
        endwin();
        return(false);
    }

    wordset.wctext_state = malloc( (wordset.wctext_length+1) * sizeof(uint8_t) );
    if ( (wordset.wctext_state == NULL) || !initialize_wordset_state() ){
        endwin();
        return(false);
    }

    if( !initialize_wordset_segments() || wordset.seg_start == wordset.seg_end){
        endwin();
        return(false);
    }

    return(true);
}

bool initialize_wordset_state(void)
{
    uint32_t index = 0;
    wchar_t* iter = wordset.wctext;

    while(index < wordset.wctext_length){
        if(index == 0)
            *(wordset.wctext_state+index) = WC_CURSOR;
        /* TODO: add macros for space keys */
        else if(*(iter+index) == '\040')
            *(wordset.wctext_state+index) = WC_CHECKPOINT_OFF;
        else
            *(wordset.wctext_state+index) = WC_OUT_OF_REACH;

        index++;
    }

    wordset.wctext_cursor = 0;

    return(true);
}

bool initialize_wordset_segments(void)
{
    uint32_t x;

    for(x = 1; x < wordset.wctext_length && *(wordset.wctext_state+x) != WC_CHECKPOINT_OFF; x++) ;

    wordset.seg_end = x;

    /* printw("%d %d", wordset.seg_start, wordset.seg_end); */
    return(true);
}

bool initialize_textview(void)
{
    /* TODO: WIDTH might need to be buf_len, ie WIDTH-2 */ 
    textview.total_rows = wordset.wctext_length/WIDTH;
    if(wordset.wctext_length%WIDTH != 0)
        textview.total_rows += 1;
    
    textview.cols   = WIDTH;
    /* TODO: +1 row index starts at 0 */
    textview.rows   = (textview.total_rows) > MAX_HEIGHT ? MAX_HEIGHT : textview.total_rows;
    textview.offset = 0;

    if( (textview.total_rows < 1) || (textview.cols < 1) ){
        endwin();
        fprintf(stderr, "  wordsetlen: %d\n  totalrow: %d\n  cols: %d\n  rows: %d\n", 
                wordset.wctext_length, textview.total_rows, textview.cols, textview.rows);
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
    if(get_ps() == PS_OUTSET)
        write_textview_wordset_wctext();
    else
        write_textview_wordset_wcextended();
    /* prefresh(WINDOW* pad, int pad_y, int pad_x, int topleft_y, 
     *          int topleft_x, int botright_y, int botright_x) */
    prefresh(textview.win , textview.offset , 0 , 
             OFFSET_Y , OFFSET_X + 1 ,
             OFFSET_Y + textview.rows , OFFSET_X + (WIDTH-2) );

}

void draw_prompt(void)
{
    wrefresh(prompt.win);
}

bool compare_segments(void)
{
    bool iscomplete = true;
    wchar_t* iter = prompt.buffer;

    for(int x = wordset.seg_start; x <= wordset.seg_end; x++, iter++){
        if( (wint_t)(*(wordset.wcextended+x)->chars) != (wint_t)(*iter)){
            iscomplete = false;
            break;
        }
    }

    return(iscomplete);
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
        /* TODO: move elsewhere */
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

    update_state(&key);
    draw_textview();

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

        update_state(NULL);
        draw_textview();
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

void update_state(wint_t* key)
{
    if (key == NULL){
        // backspace logic
        if(wordset.wctext_cursor > 0){
            *(wordset.wctext_state+wordset.wctext_cursor) = WC_OUT_OF_REACH;

            wordset.wctext_cursor--;
            *(wordset.wctext_state+wordset.wctext_cursor) = WC_CURSOR;
        }
    /* TODO: implement space testing */
    } else if (*key == KEY_SPACE && *((wordset.wcextended+wordset.wctext_cursor)->chars) == (wchar_t)KEY_SPACE){
        // compare segment logic
        if(compare_segments()){
        /*     clearbuffsir() */
        /*     WC_CHECKPOINT_ON */
        /*     new_segs() */
            *(wordset.wctext_state+wordset.wctext_cursor) = WC_CORRECT;
        }else{
        /*     failstatesir() */
        /*     *(wordset.wctext_state+wordset.wctext_cursor) = WC_INCORRECT; */
            *(wordset.wctext_state+wordset.wctext_cursor) = WC_INCORRECT;
        }
        wordset.wctext_cursor++;
        *(wordset.wctext_state+wordset.wctext_cursor) = WC_CURSOR;
    } else{
        // compare key to char logic
        /* (wordset.wcextended+wordset.wctext_cursor)->attr = WA_REVERSE; */
        if( *((wordset.wcextended+wordset.wctext_cursor)->chars) == (wchar_t)(*key) ){
            *(wordset.wctext_state+wordset.wctext_cursor) = WC_CORRECT;
        }else{
            *(wordset.wctext_state+wordset.wctext_cursor) = WC_INCORRECT;
        }

        wordset.wctext_cursor++;
        *(wordset.wctext_state+wordset.wctext_cursor) = WC_CURSOR;
    }
}

void write_textview_wordset_wctext(void)
{
    /* TODO: this needs to output wchar */
    wclear(textview.win);
    waddwstr(textview.win, wordset.wctext);
    /* for(int x = 0; x < wordset.text_length; x++){ */
        /* if( *(wordset.text_state+x) == WC_CURSOR) */
        /*     waddch(textview.win, WA_REVERSE | *(wordset.text+x)); */
        /* else */
        /*     waddch(textview.win, *(wordset.text+x)); */
}

void write_textview_wordset_wcextended(void)
{
    wclear(textview.win);
    for(unsigned int x = 0; x < wcslen(wordset.wctext); x++){
        /* if( *(wordset.wctext_state+x) == WC_CORRECT ) */
        /*     (wordset.wcextended+x)->attr = COLOR_PAIR(1); */
        switch(*(wordset.wctext_state+x)){
            case WC_CURSOR:
                (wordset.wcextended+x)->attr = WA_REVERSE;
                break;
            case WC_CORRECT:
                (wordset.wcextended+x)->attr = COLOR_PAIR(1);
                break;
            case WC_INCORRECT:
                (wordset.wcextended+x)->attr = COLOR_PAIR(2);
                break;
            case WC_OUT_OF_REACH:
                (wordset.wcextended+x)->attr = WA_NORMAL;
                break;
        }
        wadd_wch(textview.win,wordset.wcextended+x);
    }
}

void write_prompt(void)
{
    mvwaddwstr(prompt.win,1,1,prompt.buffer);
}

bool convert_to_wordset_wcextended(void)
{
    /* cchar_t* wideboys = malloc( (wcslen(wc_sentence)+1) * sizeof(cchar_t) ); */
    wordset.wcextended = malloc( (wcslen(wordset.wctext)+1) * sizeof(cchar_t) );
    /* for(int x = 0; x < wcslen(wc_sentence); x++){ */
    for(unsigned int x = 0; x < wcslen(wordset.wctext); x++){
        if( x == 0 )
            setcchar((wordset.wcextended+x),(wordset.wctext+x),WA_REVERSE,0,NULL);
        else if( *(wordset.wctext+x) == '\012')
            setcchar((wordset.wcextended+x),L"\n",0,0,NULL);
        else
            setcchar((wordset.wcextended+x),(wordset.wctext+x),0,0,NULL);
    }
    /*     if( *(wc_sentence+x) == '\012') */
    /*         setcchar((wideboys+x),newline,0,0,NULL); */
    /*     else */
    /*         setcchar((wideboys+x),(wc_sentence+x),WA_UNDERLINE,0,NULL); */
    /* } */
    return(true);
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

void free_wordset_wctext(void)
{
    free(wordset.wctext_state);
    free(wordset.wcextended);
    free(wordset.wctext);
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
