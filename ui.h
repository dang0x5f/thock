#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define KEY_ESCAPE '\033'
#define KEY_SPACE  '\040'

bool initialize_stdscr(void);
bool initialize_wordset(void);
bool initialize_wordset_state(void);
bool initialize_textview(void);
bool initialize_prompt(void);

    void reset_wordset(void);

void draw_stdscr(void);
void draw_textview(void);
void draw_textview_wordset(void);
void draw_prompt(void);

wint_t get_keycode(void);
void do_resize(void);

bool too_small(void);

void free_stdscr_resize(void);
void free_stdscr_exit(void);
void free_wordset(void);
void free_textview(void);
void free_buffer(void);
void free_prompt(void);

/* void init_ncurses(void); */
/* void reset_ncurses(void); */
/* void exit_ncurses(void); */

/* int get_key(uint8_t*); */
/* void evaluate_key(wint_t); */
/* void redraw_all(void); */

/* void init_textview(int); */
/* void load_wordset_textview(wchar_t*,int,uint8_t*); */
/* int write_to_textview(wchar_t*,uint8_t*); */

/* void init_prompt_window(void); */
/* void init_prompt_attributes(void); */
/* int write_to_prompt(wint_t); */
/* void reinit_textview(void); */
/* void reinit_prompt_window(void); */

/* void restore_wordset_buffer_data(void); */
/* void refresh_x3(void); */

#endif // UI_H
