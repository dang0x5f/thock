#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <ncurses.h>
#include <assert.h>

#include "ui.h"
#include "util.h"
#include "config.h"

#define WIDTH 50
#define MAX_HEIGHT 5

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
    uint8_t  cursor;
    uint8_t* state;
    uint32_t length;
    uint8_t  seg_start;
    uint8_t  seg_end;
    wchar_t* wctext;
    cchar_t* wcextended;
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
    wordset.wcextended    = NULL;
    wordset.state  = NULL;
    wordset.length = 0;

    /* explicitly declare modules[0] ? */
    wordset.wctext = modules[1].get_wordset(&wordset.length);
    if( (wordset.wctext == NULL) || (wordset.length == 0) ){
        endwin();
        return(false);
    }

    sanitize_nl();
    insert_nl();

    wordset.state = malloc( (wordset.length+1) * sizeof(uint8_t) );
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

bool initialize_wordset_state(void)
{
    uint32_t index = 0;
    wchar_t* iter = wordset.wctext;

    while(index < wordset.length){
        if(index == 0)
            *(wordset.state+index) = WC_CURSOR;
        /* TODO: replace magic numbers */
        else if(*(iter+index) == '\040' || *(iter+index) == '\012')
            *(wordset.state+index) = WC_WHITESPACE;
        else
            *(wordset.state+index) = WC_OUT_OF_REACH;

        index++;
    }

    wordset.cursor = 0;

    return(true);
}

bool initialize_wordset_segments(void)
{
    uint32_t x;
    wordset.seg_start = 0;
    wordset.seg_end   = 0;

    for(x = 1; x < wordset.length && *(wordset.state+x) != WC_WHITESPACE; x++) ;

    wordset.seg_end = x;

    return(true);
}

bool initialize_textview(void)
{
    /* WIDTH might need to be buf_len, ie WIDTH-2 */ 
    textview.total_rows = wordset.length/WIDTH;
    if(wordset.length%WIDTH != 0)
        textview.total_rows += 1;
    
    textview.cols   = WIDTH;
    textview.rows   = (textview.total_rows) > MAX_HEIGHT ? MAX_HEIGHT : textview.total_rows;
    textview.offset = 0;

    if( (textview.total_rows < 1) || (textview.cols < 1) ){
        endwin();
        fprintf(stderr, "  wordsetlen: %d\n  totalrow: %d\n  cols: %d\n  rows: %d\n", 
                wordset.length, textview.total_rows, textview.cols, textview.rows);
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
    prompt.buffer_length = (WIDTH-2);
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

    nodelay(prompt.win,TRUE);
    keypad(prompt.win,TRUE);
    box(prompt.win,0,0);

    return(true);
}

void sanitize_nl(void)
{
    for(uint32_t x = 0; x < wordset.length; x++){
        if((wint_t)(*(wordset.wctext+x)) == '\012')
            *(wordset.wctext+x) = (wchar_t)'\040';
    }
}

void insert_nl(void)
{
    uint32_t linelength = (WIDTH-2);
    uint32_t offset     = (linelength-1); // 0-based indexing
    
    while(offset < wordset.length){

        if((wint_t)(*(wordset.wctext+offset)) != '\040'){
            uint32_t tempindex = offset - 1;

            while((wint_t)(*(wordset.wctext+tempindex)) != '\040') tempindex--;

            *(wordset.wctext+tempindex) = (wchar_t)'\012';

            offset = tempindex;
        }

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
    memset(prompt.buffer,KEY_SPACE,sizeof(wchar_t)*prompt.buffer_length);
    write_prompt(); 
    memset(prompt.buffer,'\000',sizeof(wchar_t)*prompt.buffer_length);
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
    /* prefresh(WINDOW* pad, int pad_y, int pad_x, int topleft_y, 
     *          int topleft_x, int botright_y, int botright_x) */
    prefresh(textview.win , textview.offset , 0 , 
             OFFSET_Y , OFFSET_X + 1 ,
             OFFSET_Y + textview.rows - 1, OFFSET_X + (WIDTH-2) );

}

void draw_prompt(void)
{
    wrefresh(prompt.win);
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
    wchar_t* iter = prompt.buffer;

    for(int x = wordset.seg_start; x <= wordset.seg_end; x++, iter++){
        /* this covers for newlines in wordset and treats them as spaces \040 */
        if( (wint_t)(*(wordset.wcextended+x)->chars) == '\012' )
            continue;

        if( (wint_t)(*(wordset.wcextended+x)->chars) != (wint_t)(*iter)){
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
    ProgramState state = get_ps();

    if(!(state == PS_TOOSMOL)) save_ps(get_ps());

    if( STD_X < (WIDTH+4) || STD_Y < ((textview.rows + prompt.rows)+8) ){
        if(!(state == PS_TOOSMOL)) set_ps(PS_TOOSMOL);
        return(true);
    }
    else{
        if(state == PS_TOOSMOL) restore_ps();
        return(false);
    }
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
        case '\010':
        case KEY_BACKSPACE:
            if(!(get_ps() == PS_TOOSMOL)) backspace_buffer();
            key = WEOF;
            break;
    }
    return(key);
}

bool use_keycode(wint_t key)
{
    bool set_completed = false;

    if(key < 32) return(set_completed);
    if(get_ps() == PS_TOOSMOL) return(set_completed);
    if(prompt.buffer_index == prompt.buffer_length) return(set_completed);
    update_buffer(key);
    write_prompt();

    if(update_state(&key))
        set_completed = true;
    draw_textview();

    return(set_completed);
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

bool update_state(wint_t* key)
{
    bool set_complete = false;

    if (key == NULL){
        // backspace logic
        if(get_ps() == PS_FAILING && get_fi() == wordset.cursor){
            set_ps(PS_INSET);
            set_fi(FI_OFF);
        }

        if(wordset.cursor > 0){
            if( *((wordset.wcextended+wordset.cursor)->chars) == (wchar_t)KEY_SPACE )
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
    } else if (*key == KEY_SPACE && 
               ( ( *((wordset.wcextended+wordset.cursor)->chars) == (wchar_t)KEY_SPACE) 
               ||
               ( *((wordset.wcextended+wordset.cursor)->chars) == (wchar_t)'\012'   ) )
              ){
        // compare segment logic
        if(compare_segments()){
            if(wordset.seg_end == wordset.length-1)
                set_complete = true;
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
        if( *((wordset.wcextended+wordset.cursor)->chars) == (wchar_t)(*key) ){
            *(wordset.state+wordset.cursor) = WC_CORRECT;
        }else{
            *(wordset.state+wordset.cursor) = WC_INCORRECT;
        }

        wordset.cursor++;
        *(wordset.state+wordset.cursor) = WC_CURSOR;
    }

    return(set_complete);
}

// +1 because cursor has not advanced yet
void update_segments(void)
{
    uint32_t x;
    wordset.seg_start = wordset.cursor + 1;
    for(x = wordset.cursor + 1; x < wordset.length && *(wordset.state+x) != WC_WHITESPACE; x++) ;
    wordset.seg_end = x;
}

void write_textview_wordset_wctext(void)
{
    wclear(textview.win);
    waddwstr(textview.win, wordset.wctext);
}

void write_textview_wordset_wcextended(void)
{
    wclear(textview.win);
    for(unsigned int x = 0; x < wcslen(wordset.wctext); x++){
        switch(*(wordset.state+x)){
            case WC_CURSOR:
                (wordset.wcextended+x)->attr = WA_REVERSE;
                break;
            case WC_CORRECT:
                (wordset.wcextended+x)->attr = COLOR_PAIR(2);
                break;
            case WC_INCORRECT:
                (wordset.wcextended+x)->attr = COLOR_PAIR(1);
                break;
            case WC_OUT_OF_REACH:
            case WC_WHITESPACE:
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
    wordset.wcextended = malloc( (wcslen(wordset.wctext)+1) * sizeof(cchar_t) );
    for(unsigned int x = 0; x < wcslen(wordset.wctext); x++){
        if( x == 0 )
            setcchar((wordset.wcextended+x),(wordset.wctext+x),WA_REVERSE,0,NULL);
        else if( *(wordset.wctext+x) == '\012')
            setcchar((wordset.wcextended+x),L"\n",0,0,NULL);
        else
            setcchar((wordset.wcextended+x),(wordset.wctext+x),0,0,NULL);
    }
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
