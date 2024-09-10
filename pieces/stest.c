#include <ncurses.h>
#include <string.h>

WINDOW* prompt;         // PROMPT ; TODO: should this be global?
int x, y;               // PROMPT PROPERTY

enum { CURSOR, SAVEON, SAVEW8, NEUTRAL, CORRECT, INCORRECT };       // FLAGS FOR TEXT CHAR STATES

int write_text(char* text, char state[], int text_index){   // ******** START WRITE TEXT **********
    getyx(prompt,y,x);
    /* printw("Y: %d X: %d", y,x); */
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
}                                                           // *********** END WRITE TEXT ***************

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

    state[index + 1] = CURSOR;

    return(index + 1);
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
        waddch(prompt,c);
        r = ((buffer_index == 50 - 2 - 1) ? (50 - 2 - 1) : (buffer_index + 1));
    }
       
    return r; 
}

int main(void){

    int prows = 3;              // PROMPT PROPERTY
    int pcols = 50;             // PROMPT PROPERTY

    char  buffer[pcols - 2];    // PROMPT PROPERTY
    int buffer_index = 0;       // PROMPT PROPERTY

    char* text = "one two three";       // DATA-SRC PROPERTY
    char  text_state[strlen(text)];     // DATA-SRC PROPERTY
    int text_index = 0;                 // DATA-SRC PROPERTY

    int c;                      // CHAR INPUT

    initscr();                                      // INIT
    noecho();                                       // INIT
    start_color();                                  // INIT
    init_pair(1, COLOR_BLACK, COLOR_GREEN);         // INIT
    init_pair(2, COLOR_BLACK, COLOR_RED);           // INIT
    refresh();                                      // INIT

    prompt = newwin(prows, pcols, 5, 10);       // PROMPT INIT
    box(prompt,0,0);                            // PROMPT INIT
    keypad(prompt, TRUE);                       // PROMPT INIT
    wmove(prompt,1,1);                          // PROMPT INIT (SETTING CURSOR)
    wrefresh(prompt);                           // PROMPT INIT

    refresh();                  // INIT (ESSENTIALLY START OPTION)
    getch();                    // INIT (ESSENTIALLY START OPTION)


    init_text_state(text,text_state);                                                       // TEXT INIT

    while(true){                                      // *********** TYPING TEST START ********************

        write_text(text,text_state,text_index);                                             // UPDATE PROGRESS
        text_index = set_text_state(text, text_state, c = wgetch(prompt), text_index);      // GET AND ASSESS INPUT
        buffer_index = write_char(c, buffer, buffer_index, text_state, &text_index);        // WRITE INPUT

        /* printw("%d",text_index); */                                                      // SANITY CHECK DEBUG
        /* mvwprintw(stdscr,20,20,"%d",text_index); */                                      // SANITY CHECK DEBUG
    }                                                 // *************** TYPING TEST END ************************

    endwin();

    return(0);
}
