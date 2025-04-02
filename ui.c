/* TODO */
/* error counter */ 
/* reset Ctrl+R */
/* timer */

/* #define _XOPEN_SOURCE_EXTENDED 1 */
#include <stdio.h>
#include <ncurses.h>
#include <assert.h>

#include "ui.h"
#include "util.h"
#include "config.h"

#define WIDTH 50
#define MAX_HEIGHT 5

#define STD_X         ( getmaxx(stdscr) )
#define STD_Y         ( getmaxy(stdscr) )
#define BORDERPAD_X   ( (STD_X / 2) - (WIDTH / 2) )
#define BORDERPAD_Y   ( (STD_Y - (textview.rows + prompt.rows) ) / 2 )


typedef struct {
    int rows;
    int cols;
    int total_rows;       
    int xoffset;
    int yoffset;
    WINDOW*  win;
} TextView;

typedef struct {
    int* state;
    int  cursor;
    int  length;
    int  seg_start;
    int  seg_end;
    char*  wctext;
    /* cchar_t*  wcextended; */
} Wordset;

typedef struct {
    uint8_t  cols;
    uint8_t  rows;
    uint8_t  xpos;
    uint8_t  ypos;
    uint8_t  buffer_index;
    uint8_t  buffer_length;
    char* buffer;
    WINDOW*  win;
} Prompt;

typedef struct {
    int errs;
} Stats;


/* Globals */
static TextView textview;
static Wordset  wordset;
static Prompt   prompt;
static Stats    stats;


bool initialize_stdscr(void)
{
    if(!initscr())
        return(false);

    noecho();
    set_escdelay(25);
    start_color();
    toggle_cursor(CVS_INVISIBLE);

    /* TODO: init color pairs separately */
    init_pair(1,COLOR_BLACK,COLOR_RED);
    init_pair(2,COLOR_BLACK,COLOR_GREEN);
    init_pair(3,COLOR_BLACK,COLOR_BLUE);

    draw_stdscr();
    
    return(true);
}

bool initialize_wordset(void)
{
    wordset.wctext        = NULL;
    /* wordset.wcextended    = NULL; */
    wordset.state  = NULL;
    wordset.length = 0;
    textview.total_rows = 0;

    /* TODO: explicitly declare modules[0] ? */
    wordset.wctext = modules[0].get_wordset(&wordset.length);
    if( (wordset.wctext == NULL) || (wordset.length == 0) ){
        endwin();
        return(false);
    }

    sanitize_nl();
    insert_nl();

    wordset.state = malloc( (wordset.length) * sizeof(int) );
    if ( (wordset.state == NULL) || !initialize_wordset_state() ){
        endwin();
        return(false);
    }

    if( !initialize_wordset_segments() || wordset.seg_start == wordset.seg_end){
        endwin();
        return(false);
    }

    return(true);
}
/* static uint32_t debug_sz; */
bool initialize_wordset_state(void)
{
    int index = 0;
    char* iter = wordset.wctext;

    while(index < wordset.length){
        if(index == 0)
            *(wordset.state+index) = WC_CURSOR;
        /* TODO: replace magic numbers */
        else if(*(iter+index) == (char)'\040' || *(iter+index) == (char)'\012')
            *(wordset.state+index) = WC_WHITESPACE;
        else
            *(wordset.state+index) = WC_OUT_OF_REACH;

        index++;
    }
    /* debug_sz = index; */

    wordset.cursor = 0;

    return(true);
}

bool initialize_wordset_segments(void)
{
    int x;
    wordset.seg_start = 0;
    wordset.seg_end   = 0;

    for(x = 1; x < wordset.length && *(wordset.state+x) != WC_WHITESPACE; x++) ;

    wordset.seg_end = x;

    return(true);
}

bool initialize_textview(void)
{
    if(wordset.length%(WIDTH-2) != 0)
        textview.total_rows += 1;
/* printw(" , %lu ", textview.total_rows); */
    
    textview.cols    = WIDTH;
    textview.rows    = (textview.total_rows) >= MAX_HEIGHT ? MAX_HEIGHT : textview.total_rows;
    textview.yoffset = 0;
    textview.xoffset = 0;

    if( (textview.total_rows < 1) || (textview.cols < 1) ){
        endwin();
        fprintf(stderr, "  wordsetlen: %d\n  totalrow: %d\n  cols: %d\n  rows: %d\n", 
                wordset.length, textview.total_rows, textview.cols, textview.rows);
        return(false);
    }

    textview.win = newpad(textview.total_rows, WIDTH-2);

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
    prompt.buffer_length = (WIDTH-2);
    prompt.buffer = malloc( (prompt.buffer_length+1) * sizeof(char) );
    if(!prompt.buffer){
        endwin();
        fprintf(stderr,"prompt.buffer null");
        return(false);
    }

    prompt.win = newwin(prompt.rows, prompt.cols,
                        BORDERPAD_Y+textview.rows, BORDERPAD_X);
    if(!prompt.win){
        endwin();
        fprintf(stderr,"prompt.win null");
        return(false);
    }

    nodelay(prompt.win,TRUE);
    keypad(prompt.win,TRUE);
    box(prompt.win,0,0);

    return(true);
}

bool initialize_stats(void)
{
    stats.errs = 0;

    return(true);
}

void sanitize_nl(void)
{
    for(int x = 0; x < wordset.length; x++){
        if((int)(*(wordset.wctext+x)) == (int)'\012')
            *(wordset.wctext+x) = (char)'\040';
    }
}

void insert_nl(void)
{
    int linelength = (WIDTH-2);
    int offset     = (linelength-1); // 0-based indexing
    
    while(offset < wordset.length-1){     // dont needlessly make last character \n

        if((int)(*(wordset.wctext+offset)) != (int)'\040'){
            int tempindex = offset - 1;

            while((int)(*(wordset.wctext+tempindex)) != (int)'\040') tempindex--;

            *(wordset.wctext+tempindex) = (char)'\012';

            offset = tempindex;
        }else if((int)(*(wordset.wctext+offset)) == (int)'\040'){
            *(wordset.wctext+offset) = (char)'\012';
        }

        textview.total_rows += 1;
        offset += linelength;
    }

}

void reset_prompt(void)
{
    reset_buffer();    
    prompt.buffer_index = 0;
    place_cursor();    
}

void reset_buffer(void)
{
    int x;

    for(x = 0; x < prompt.buffer_length-1; x++)
        *(prompt.buffer+x) = (char)'\040';
    *(prompt.buffer+x) = (char)'\0';

    write_prompt(); 
}

void draw_stdscr(void)
{
    box(stdscr,0,0);
    draw_colorscheme();
    wrefresh(stdscr);
}

void draw_textview(void)
{
    if(get_ps() == PS_OUTSET)
        write_textview_wordset_wctext();
    else
        write_textview_wordset_wcextended();
    /* else */
    /*     write_textview_wordset_wcextended(); */
    /* prefresh(WINDOW* pad, int pad_y, int pad_x, int topleft_y, 
     *          int topleft_x, int botright_y, int botright_x) */
    prefresh(textview.win , textview.yoffset , 0 , 
             BORDERPAD_Y , BORDERPAD_X + 1 ,
             BORDERPAD_Y + textview.rows - 1, BORDERPAD_X + (WIDTH-2) +1 );

}

void draw_prompt(void)
{
    wrefresh(prompt.win);
}

void draw_stats()
{
    /* TODO */
}

void draw_colorscheme(void)
{
    move(1,2);
    addch(COLOR_PAIR(1) | ' ');
    addch(COLOR_PAIR(2) | ' ');
    addch(COLOR_PAIR(3) | ' ');
}

bool compare_segments(void)
{
    bool iscomplete = true;
    char* iter = prompt.buffer;

    for(int x = wordset.seg_start; x <= (int)wordset.seg_end; x++, iter++){
        /* this covers for newlines in wordset and treats them as spaces \040 */
        if( (int)(*(wordset.wctext+x)) == (int)'\012' )
            continue;

        if( (int)(*(wordset.wctext+x)) != (int)(*iter)){
            iscomplete = false;
            break;
        }
    }

    return(iscomplete);
}

void do_resize(void)
{
    free_textview();
    free_prompt();
    free_stdscr_resize();

    if(too_small()){
        mvprintw(1,1,"too smol...");
        draw_stdscr();
    }else{
        textview.win = newpad(textview.total_rows, WIDTH);
        if(textview.win == NULL){
            endwin();
            fprintf(stderr, "textview.win null\n");
            /* return(false); */
        }

        prompt.win = newwin(prompt.rows, prompt.cols,
                            BORDERPAD_Y + textview.rows, BORDERPAD_X);
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
    ProgramState state = get_ps();

    if(!(state == PS_TOOSMOL)) save_ps(get_ps());

    if( STD_X < (WIDTH+4) || STD_Y < (int)((textview.rows + prompt.rows)+8) ){
        if(!(state == PS_TOOSMOL)) set_ps(PS_TOOSMOL);
        return(true);
    }
    else{
        if(state == PS_TOOSMOL) restore_ps();
        return(false);
    }
}

int get_keycode(void)
{
    /* waddch(prompt.win,'g'); */
    int key = 0;
    if((key = wgetch(prompt.win)) == ERR)
        return(key);

    switch(key){
        case KEY_RESIZE:
            do_resize();
            break;
        case KEY_ESCAPE:
            flushinp();
            break;
        case '\010':
        case KEY_BACKSPACE:
            if(!(get_ps() == PS_TOOSMOL)) backspace_buffer();
            break;
    }
    return(key);
}

bool use_keycode(int key)
{
    bool set_completed = false;

    if(key < 32 || key == KEY_BACKSPACE) return(set_completed);
    if(get_ps() == PS_TOOSMOL) return(set_completed);
    if(prompt.buffer_index == prompt.buffer_length) return(set_completed);
    update_buffer(key);
    write_prompt();

    if(update_state(&key))
        set_completed = true;
    draw_textview();
    draw_stats();

    /* fprintf(stderr," <%d,%d> ", wordset.seg_start, wordset.seg_end); */

    return(set_completed);
}

void scroll_down(void)
{
    textview.yoffset += 1;
}

/* ... [48] [49] [50] */

void backspace_buffer(void)
{
    if(prompt.buffer_index > 0){
        prompt.buffer_index -= 1;
        mvwaddch(prompt.win,1,prompt.buffer_index+1,' '|A_NORMAL);
        /* mvwaddwstr(prompt.win,1,prompt.buffer_index+1,L" "); */
        /* mvwaddch(prompt.win,1,prompt.buffer_index+1,KEY_SPACE); */
        *(prompt.buffer+prompt.buffer_index) = (char)'\000';
        write_prompt();

        update_state(NULL);
        draw_textview();
    }
}

void update_buffer(int key)
{
    if(prompt.buffer_index == prompt.buffer_length){
        *(prompt.buffer+prompt.buffer_index-1) = key;
        *(prompt.buffer+prompt.buffer_length)  = (char)'\000';
    }else{
        *(prompt.buffer+prompt.buffer_index) = key;
        prompt.buffer_index += 1;
        *(prompt.buffer+prompt.buffer_index) = (char)'\000';
    }
}

bool update_state(int* key)
{
    bool set_complete = false;

    if (key == NULL){
        // backspace logic
        if(get_ps() == PS_FAILING && get_fi() == (int)wordset.cursor){
            set_ps(PS_INSET);
            set_fi(FI_OFF);
        }

        if(wordset.cursor > 0){
            if( *(wordset.wctext+wordset.cursor) == (char)KEY_SPACE ||
                *(wordset.wctext+wordset.cursor) == (char)'\012' )
                *(wordset.state+wordset.cursor) = WC_WHITESPACE;
            else
                *(wordset.state+wordset.cursor) = WC_OUT_OF_REACH;

            wordset.cursor--;
            *(wordset.state+wordset.cursor) = WC_CURSOR;
        }
      
    } else if(get_ps() == PS_FAILING){
        *(wordset.state+wordset.cursor) = WC_INCORRECT;
        wordset.cursor++;
        *(wordset.state+wordset.cursor) = WC_CURSOR;
    } else if (*key == KEY_SPACE && (int)*(wordset.wctext+wordset.cursor) <= KEY_SPACE
               /* ( ( *((wordset.wcextended+wordset.cursor)->chars) == (char)KEY_SPACE) */ 
               /* || */
               /* (   *((wordset.wcextended+wordset.cursor)->chars) == (char)'\012'   ) ) */
              ){
        // compare segment logic
        if(compare_segments()){
            if(wordset.seg_end == wordset.length-1)
                set_complete = true;

            if(   (textview.total_rows > MAX_HEIGHT)                                 && 
                  ( (textview.yoffset+(MAX_HEIGHT-1)) != (textview.total_rows-1) )   && 
                  (*(wordset.wctext+wordset.cursor) == (char)'\012') ){

                scroll_down();

            }

            update_segments();
            reset_prompt();
            *(wordset.state+wordset.cursor) = WC_CORRECT;
        }else{
            set_ps(PS_FAILING);
            set_fi(wordset.cursor);
            *(wordset.state+wordset.cursor) = WC_INCORRECT;
        }
        wordset.cursor++;
        *(wordset.state+wordset.cursor) = WC_CURSOR;
    } else{
        // compare key to char logic
        if( *(wordset.wctext+wordset.cursor) == (char)(*key) ){
            *(wordset.state+wordset.cursor) = WC_CORRECT;
        }else{
            *(wordset.state+wordset.cursor) = WC_INCORRECT;
            stats.errs += 1;
        }

        wordset.cursor++;
        *(wordset.state+wordset.cursor) = WC_CURSOR;
    }

    return(set_complete);
}

void update_segments(void)
{
    int x;
    wordset.seg_start = wordset.cursor + 1; // cursor has not advanced yet
    for(x = wordset.cursor + 1; x < wordset.length && *(wordset.state+x) != WC_WHITESPACE; x++) ;
    wordset.seg_end = x;
}

void write_textview_wordset_wctext(void)
{
    wclear(textview.win);
    waddstr(textview.win, wordset.wctext);
}

void write_textview_wordset_wcextended(void)
{
    wclear(textview.win);
    for(int x = 0; x < wordset.length; x++){
        switch(*(wordset.state+x)){
            case WC_CURSOR:
                waddch(textview.win,*(wordset.wctext+x) | A_REVERSE);
                break;
            case WC_CORRECT:
                waddch(textview.win,*(wordset.wctext+x) | COLOR_PAIR(2));
                break;
            case WC_INCORRECT:
                waddch(textview.win,*(wordset.wctext+x) | COLOR_PAIR(1));
                break;
            case WC_OUT_OF_REACH:
            case WC_WHITESPACE:
                waddch(textview.win,*(wordset.wctext+x) | A_NORMAL);
                break;
        }
    }
}

void write_prompt(void)
{
    mvwaddstr(prompt.win,1,1,prompt.buffer);
}

bool convert_to_wordset_wcextended(void)
{
    /* wordset.wcextended = malloc( wordset.length * sizeof(cchar_t) ); */
    /* for(unsigned int x = 0; x < wordset.length; x++){ */
    /*     if( x == 0 ) */
    /*         setcchar((wordset.wcextended+x),(wordset.wctext+x),WA_REVERSE,0,NULL); */
    /*     else if( *(wordset.wctext+x) == '\012') */
    /*         setcchar((wordset.wcextended+x),L"\n",0,0,NULL); */
    /*     else */
    /*         setcchar((wordset.wcextended+x),(wordset.wctext+x),0,0,NULL); */
    /* } */
    return(true);
}

void place_cursor(void)
{
    wmove(prompt.win,1,1+prompt.buffer_index);
}

/* TODO: add global current cursor. assign it here ? */
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
    free(wordset.state);
    /* free(wordset.wcextended); */
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
