#ifndef UI_H
#define UI_H

#include <wchar.h>

#define KEY_ESC 27

void init_ncurses(void);
void reset_ncurses(void);
void exit_ncurses(void);

int get_key(void);
void evaluate_key(wint_t);
void redraw_all(void);

void init_textview(int);
void load_wordset_textview(wchar_t*,int);
int write_to_textview(wchar_t*);

void init_prompt_window(void);
void init_prompt_attributes(void);
int write_to_prompt(wint_t);
void reinit_textview(void);
void reinit_prompt_window(void);

void restore_wordset_buffer_data(void);
void refresh_x3(void);

#endif // UI_H
