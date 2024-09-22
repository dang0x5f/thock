/* TODO: backspacing should require erasing everything back   */
/*       to buffer/prompt beginning.                          */ 
/*       this might require reworking the logic of checking   */
/*       segments, for example temp string of buffer could be */
/*       compared with temp string of text. Both extracted as */
/*       dynamically allocated strings.                       */

#include <ncurses.h>
#include <string.h>

WINDOW* prompt;
int x, y;
int seg_start, seg_end;

enum { CURSOR, SAVEON, SAVEW8, NEUTRAL, CORRECT, INCORRECT };

#define KEY_SPACE 32
int fail_state =  0;
int fail_index = -1;

int clear_prompt_buffer(char [], int);
int set_new_segments(char [], int, int);
int check_segments(char []);
int init_seg(char [], int);
int write_text(char* , char [], int);
int init_text_state(char* , char []);
int set_text_state(char* , char [], int , int, char [], int*);
int write_char(int , char [], int);

int clear_prompt_buffer(char buffer[], int len){
    for(int x = 0; x < len; x++)
        buffer[x] = '\0';
    flushinp();
    
    wmove(prompt,1,1);
    for(int x = 1; x <= len; x++){
        waddch(prompt,' ');
    }

    wmove(prompt,1,1);
    wrefresh(prompt);

    return(0);
}

int set_new_segments(char state[], int start, int len){
    int x;
    seg_start = start;
    for(x = start; x < len && state[x] != SAVEW8; x++);
    seg_end = x;
    /* printw("start: %d end: %d", seg_start, seg_end); */

    return(0);
}

int check_segments(char state[]){
    int failed = -1;
    for(int x = seg_start; x < seg_end; x++){
        if(state[x] == INCORRECT)
            failed = x;
    }
    return(failed);
}

int init_seg(char state[], int size){
    int x;
    seg_start = 0;
    for(x = 1; x < size && state[x] != SAVEW8; x++);
    seg_end = x;

    /* printw("start: %d end: %d", seg_start, seg_end); */
    return(0);
}

int write_text(char* text, char state[], int text_index){
    getyx(prompt,y,x);
    wmove(stdscr, 1,1);

    for(int x = 0; x < strlen(text); x++){
        if( state[x] == CURSOR ){
            addch(A_REVERSE | *(text+x));
        }
        else if(state[x] == SAVEON ){
            addch(COLOR_PAIR(3) | *(text+x));
        }
        else if( state[x] == SAVEW8 ){
            addch(*(text+x));
        }
        else if( state[x] == NEUTRAL ){
            addch(*(text+x));
        }
        else if( state[x] == CORRECT ){
            addch(COLOR_PAIR(1) | *(text+x));
        }
        else if( state[x] == INCORRECT ){
            addch(COLOR_PAIR(2) | *(text+x));
        }
    }
    refresh();
    wmove(prompt,y,x);
    wrefresh(prompt);

    return(0);
}

int init_text_state(char* text, char state[]){
    int index = 0;
    char* iter;

    iter = text;

    while(*iter != '\0'){
        if(index == 0)
            state[index] = CURSOR;
        else if(*iter == ' ')
            state[index] = SAVEW8;
        else
            state[index] = NEUTRAL;

        index++;
        iter = text + index;
    }

    return(0);
}

int set_text_state(char* text, char state[], int c, int index, char buffer[], int* buffer_index){
    switch(c){
        case KEY_DC:
        case KEY_IC:
        case KEY_HOME:
        case KEY_END:
        case KEY_NPAGE:
        case KEY_PPAGE:
            return(index);
            break;
        case KEY_SPACE:
            // char* text should be null terminated
            if(*(text+index) != KEY_SPACE && index == strlen(text)-1){     
                if(check_segments(state) >= 0){
                    return(index);
                }
                else{
                    printw("%s", "W");
                    *buffer_index = clear_prompt_buffer(buffer,48);
                }
            }
            else if(*(text+index) != KEY_SPACE && index < strlen(text)-1){
                state[index] = INCORRECT;
            }
            else{
                if(check_segments(state) >= 0){
                    state[index] = INCORRECT;
                    fail_state = 1;
                    fail_index = index;
                }
                else{
                    state[index] = SAVEON;
                    set_new_segments(state,index+1,strlen(text));
                    *buffer_index = clear_prompt_buffer(buffer,48);
                }
            }
            break;
        case KEY_BACKSPACE:
            if(index > seg_start){
                state[index] = NEUTRAL;
                state[index - 1] = CURSOR;
                return(index - 1);
            }
            else{
                return(index);
            }
            break;
        default:
            state[index] = (*(text+index) == c) ? CORRECT : INCORRECT;
            break;
    }

    if(index < strlen(text) - 1){
        state[index+1] = CURSOR;
        return(index+1);
    }
    else{
        return(index);
    }
}

int set_fail_state(char* text, char state[], int c, int index){

    switch(c){
        case KEY_DC:
        case KEY_IC:
        case KEY_HOME:
        case KEY_END:
        case KEY_NPAGE:
        case KEY_PPAGE:
            return(index);
            break;
        case KEY_BACKSPACE:
            if(fail_index == index)
                fail_state = 0;
            if(index > 0){
                state[index] = (*(text+index) == ' ') ? SAVEW8 : NEUTRAL;
                state[index - 1] = CURSOR;
                return(index - 1);
            }
            else{
                return(index);
            }
            break;
        default:
            state[index] = INCORRECT;
            break;
    }

    if(index < strlen(text) - 1){
        state[index+1] = CURSOR;
        return(index+1);
    }
    else{
        return(index);
    }
}

int write_char(int c, char buffer[], int buffer_index){
    int r = buffer_index;

    if(c == KEY_SPACE && r == 0)
        return(buffer_index);
    
    if(c == KEY_DC || c == KEY_IC || c == KEY_NPAGE || c == KEY_PPAGE || c == KEY_HOME || c == KEY_END)
        return(buffer_index);

    if(c == KEY_BACKSPACE){
        /* mvaddstr(20,30,"BS"); */
        if(getcurx(prompt) > 1){
            wmove(prompt, getcury(prompt),getcurx(prompt)-1);
            waddch(prompt,' ');
            wmove(prompt, getcury(prompt),getcurx(prompt)-1);
        }
        r = ((buffer_index > 0) ? (buffer_index - 1) : 0);
    }
    else if(c >= 32){
        buffer[buffer_index] = c;
        if(buffer_index == (50 - 2 - 1)){
            wmove(prompt, getcury(prompt),getcurx(prompt)-1);
            waddch(prompt,c);
            r = 50 - 2 - 1;
        }
        else{
            waddch(prompt,c);
            r = buffer_index + 1;
        }
    }
       
    return r; 
}

int main(void){
    int prows = 3;
    int pcols = 50;

    char buffer[pcols - 2];
    int  buffer_index = 0;

    char* text = "foobar foo bar f b!";
    char  text_state[strlen(text)];
    int   text_index = 0;

    int c;

    initscr();
    noecho();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_RED  );
    /* init_pair(3, COLOR_BLACK, COLOR_CYAN ); */
    init_color(33,0,0,1000);
    init_pair(3, COLOR_BLACK, 33);
    refresh();

    prompt = newwin(prows, pcols, 5, 10);
    box(prompt,0,0);
    keypad(prompt, TRUE);
    wmove(prompt,1,1);
    wrefresh(prompt);

    init_text_state(text,text_state);
    init_seg(text_state,strlen(text));

    while(true){
        write_text(text,text_state,text_index);
        c = wgetch(prompt);
        if(!fail_state)
            text_index = set_text_state(text, text_state, c, text_index, buffer, &buffer_index);
        else
            text_index = set_fail_state(text, text_state, c, text_index);
        buffer_index = write_char(c, buffer, buffer_index);

        /* printw("%d",text_index); */
        /* mvwprintw(stdscr,20,20,"%d",buffer_index); */
    }

    endwin();

    return(0);
}
