#include <ncurses.h>
#include <string.h>

int main(void){
    char* word = "dang0x5f";
    initscr();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_RED);

    mvaddstr(1,1,word);

    move(20,1); // prompt

    int x,y;
    int topind = 0;

    int ch;
    while(true){
        if( (ch=getch()) == KEY_BACKSPACE){
            move(getcury(stdscr),getcurx(stdscr));
            /* addch(' '); */
            delch();
            /* move(getcury(stdscr),getcurx(stdscr)-1); */
        }else if(ch == *(word + topind)){
            x = getcurx(stdscr);
            y = getcury(stdscr);
            move(1,1);
            for(int x = 0; x < strlen(word); x++){
                if(x == 0){
                    addch(COLOR_PAIR(1) | *(word+x));
                }else{
                    addch(*(word+x));
                }
            }
            move(y,x);
        }else if(ch != *(word + topind)){
            x = getcurx(stdscr);
            y = getcury(stdscr);
            move(1,1);
            for(int x = 0; x < strlen(word); x++){
                if(x == 0){
                    addch(COLOR_PAIR(2) | *(word+x));
                }else{
                    addch(*(word+x));
                }
            }
            move(y,x);
        }
    }

    refresh();
    getch();

    endwin();
    return(0);
}
