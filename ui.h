#ifndef UI_H
#define UI_H

#include <wchar.h>

#define KEY_ESC 27

void init_ncurses(void);
void reset_ncurses(void);
void exit_ncurses(void);

int get_key(void);
void evaluate_key(wint_t);
void redraw_all(int);

void init_textview(int);
    /* void draw_textview(void); */
void load_wordset_textview(char*, int);
int write_to_textview(char*,int);

void init_prompt(void);
    /* void draw_prompt(void); */
int write_to_prompt(wint_t);

void refresh_x3(void);

#endif // UI_H
