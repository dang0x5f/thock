#ifndef UI_H
#define UI_H

#define KEY_ESC 27

void init_ncurses(void);
void reset_ncurses(void);
void exit_ncurses(void);

wchar_t get_key(void);

void init_textview(void);
void draw_textview(void);

/* void init_textview_scroll(void); */
/* void draw_textview_scroll(void); */

void init_prompt(void);
void draw_prompt(void);

#endif // UI_H
