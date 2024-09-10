#include <ncurses.h>
#include <string.h>

WINDOW* prompt;
int x, y;

enum { CURSOR, SAVEON, SAVEW8, NEUTRAL, CORRECT, INCORRECT };

int write_text(char* text, char state[], int text_index){
    getyx(prompt,y,x);
    wmove(stdscr, 1,1);

    for(int x = 0; x < strlen(text); x++){
        if( state[x] == CURSOR ){
            addch(A_REVERSE | *(text+x));
        }
        else if( state[x] == SAVEW8 || state[x] == SAVEON){
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

int set_text_state(char* text, char state[], int c, int index){
    if(c == KEY_BACKSPACE){
        if(index > 0){
            state[index] = NEUTRAL;
            state[index - 1] = CURSOR;
            return(index - 1);
        }
        else{
            return(index);
        }
    }

    if(*(text+index) == c)
        state[index] = CORRECT;
    else
        state[index] = INCORRECT;

if(index < strlen(text)-1){
    state[index + 1] = CURSOR;
    return(index + 1);
}    
else{
    return(index);
}
    /* state[index + 1] = CURSOR; */

    /* return(index + 1); */
}

int write_char(int c, char buffer[], int buffer_index, char text_state[], int* text_index){
    int r = buffer_index;

    if(c == KEY_BACKSPACE){
        mvaddstr(20,30,"BS");
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
            /* waddch(prompt,' '); */
        waddch(prompt,c);
            /* wmove(prompt, getcury(prompt),getcurx(prompt)-1); */
            r = 50 - 2 - 1;
        }
        else{
        waddch(prompt,c);
            r = buffer_index + 1;
        }


        /* waddch(prompt,c); */
        /* buffer[buffer_index] = c; */
        /* r = ((buffer_index == 50 - 2 - 1) ? (50 - 2 - 1) : (buffer_index + 1)); */
    }
       
    return r; 
}

int main(void){

    int prows = 3;
    int pcols = 50;

    char  buffer[pcols - 2];
    int buffer_index = 0;

    char* text = "one two three";
    char  text_state[strlen(text)];
    int text_index = 0;

    int c;

    initscr();
    noecho();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    refresh();

    prompt = newwin(prows, pcols, 5, 10);
    box(prompt,0,0);
    keypad(prompt, TRUE);
    wmove(prompt,1,1);

    wrefresh(prompt);

    refresh();
    getch();
    init_text_state(text,text_state);

    while(true){
        write_text(text,text_state,text_index);
        text_index = set_text_state(text, text_state, c = wgetch(prompt), text_index);
        buffer_index = write_char(c, buffer, buffer_index, text_state, &text_index);

        /* printw("%d",text_index); */
        mvwprintw(stdscr,20,20,"%d",buffer_index);
    }

    endwin();

    return(0);
}
