#ifndef UI_H
#define UI_H

/* #include <wchar.h> */

#define KEY_ESC 27

void init_ncurses(void);
void reset_ncurses(void);
void exit_ncurses(void);

wchar_t get_key(void);
void evaluate_key(wchar_t);
void redraw_all(void);

void init_textview(void);
void draw_textview(void);
void write_to_textview(char*,char[],int);

void init_prompt(void);
void draw_prompt(void);
void write_to_prompt(int,char[],int);

#endif // UI_H
