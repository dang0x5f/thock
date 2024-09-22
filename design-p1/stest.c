#include <ncurses.h>
#include <string.h>

WINDOW* prompt;         // PROMPT ; TODO: should this be global?
int x, y;               // PROMPT PROPERTY

enum { CURSOR, SAVEON, SAVEW8, NEUTRAL, CORRECT, INCORRECT };       // FLAGS FOR TEXT CHAR STATES

int write_text(char* text, char state[], int text_index){   // ******** START WRITE TEXT **********
    getyx(prompt,y,x);                                              // SAVE PROMPT Y X
    /* printw("Y: %d X: %d", y,x); */                               // DEBUG
    wmove(stdscr, 1,1);                                             // MOVE TO TEXT BOX

    for(int x = 0; x < strlen(text); x++){                          // ITERATE OVER TEXT AND PRINT APPROPRIATE ATTRIBUTE
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
    refresh();                                                      // REFRESH STD
    wmove(prompt,y,x);                                              // RETURN CURSOR TO PROMPT
    wrefresh(prompt);                                               // REFRESH PROMPT

    return(0);                                                      // RET VALUE USELESS ATM
}                                                           // *********** END WRITE TEXT ***************

int init_text_state(char* text, char state[]){              // *********** START INIT TEXT **************
    int index = 0;                                 
    char* iter;

    iter = text;                                                    // SET ITER TO FRONT, INDEX + TEXTBASEPTR = ITER

    while(*iter != '\0'){                                           // MARK CURSOR, SPACES, AND DEFAULT
        if(index == 0)
            state[index] = CURSOR;
        else if(*iter == ' ')
            state[index] = SAVEW8;
        else
            state[index] = NEUTRAL;

        index++;
        iter = text + index;
    }

    return(0);                                                      // RET VALUE USELESS ATM
}                                                           // ************** END INIT TEXT **************

int set_text_state(char* text, char state[], int c, int index){         // ************** START SETTING TEXT STATE ******************
    if(c == KEY_BACKSPACE){                                                     // IF KEY IS BS, THEN 
        if(index > 0){                                                              // IF ANYTHING WAS TYPED (index moved passed 0), RESET IT
            state[index] = NEUTRAL;
            state[index - 1] = CURSOR;
            return(index - 1);                                                      // RETURN DECREMENTED INDEX TO GO BACK
        }
        else{
            return(index);                                                          // IF NOTHING HAS BEEN TYPED, GO NOWHERE
        }
    }

    if(*(text+index) == c)                                                      // IF COMPARISON IS EQ
        state[index] = CORRECT;                                                     // MARK STATE CORRECT
    else                                                                        // ELSE
        state[index] = INCORRECT;                                                   // MARK STATE INCORRECT

    state[index + 1] = CURSOR;                                                  // NO MATTER CORRECT/INCORRECT, ADVANCE CURSOR...

    return(index + 1);                                                          // ...THUS RETURN INCREMENTED INDEX
}                                                                       // ****************** END SETTING TEXT STATE ********************

int write_char(int c, char buffer[], int buffer_index, char text_state[], int* text_index){ // ************* START WRITE CHAR *************
    int r = buffer_index;

    if(c == KEY_BACKSPACE){                                                                         // IF BS
        mvaddstr(20,30,"BS");                                                                           // DEBUG 
        if(getcurx(prompt) > 1){                                                                        // IF PROMPT X > 1 (needs it account for box wall too)
            wmove(prompt, getcury(prompt),getcurx(prompt)-1);                                               // MOVE BACK
            waddch(prompt,' ');                                                                             // ERASE CHAR
            wmove(prompt, getcury(prompt),getcurx(prompt)-1);                                               // MOVE BACK, SINCE ADDING SPACE MOVED UP
        }

        r = ((buffer_index > 0) ? (buffer_index - 1) : 0);                                              // IF ANYTHING IN BUFFER, DECRE BUFF_IND ELSE BUFF_IND = 0
    }
    else if(c >= 32){                                                                               // ELSE IF VALID ASCII CHAR
        waddch(prompt,c);                                                                               // WRITE IT
        r = ((buffer_index == 50 - 2 - 1) ? (50 - 2 - 1) : (buffer_index + 1));                         // IF WRITING OUT OF INPUT ZONE, SET BUFF_IND TO END ELSE INCR BUFF_IND
    }
       
    return r; 
}                                                                                               // ************ END WRITE CHAR ***********

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
