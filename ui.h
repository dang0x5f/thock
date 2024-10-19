#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define KEY_ESCAPE '\033'
#define KEY_SPACE  '\040'

typedef enum {
    CVS_INVISIBLE,
    CVS_NORMAL,
    CVS_VERYVISIBLE
} CursorVisibilityState;

bool initialize_stdscr(void);
bool initialize_wordset(void);
bool initialize_wordset_state(void);
bool initialize_wordset_segments(void);
bool initialize_textview(void);
bool initialize_prompt(void);

    void reset_wordset(void);

void draw_stdscr(void);
void draw_textview(void);
void draw_prompt(void);

bool compare_segments(void);
void do_resize(void);
bool too_small(void);

wint_t get_keycode(void);
bool use_keycode(wint_t);

void backspace_buffer(void);
void update_buffer(wint_t);
void update_state(wint_t*);
void write_textview_wordset_wctext(void);
void write_textview_wordset_wcextended(void);
void write_prompt(void);

bool convert_to_wordset_wcextended(void);

void place_cursor(void);
void toggle_cursor(CursorVisibilityState);

void free_stdscr_resize(void);
void free_stdscr_exit(void);
void free_wordset_wctext(void);
void free_textview(void);
void free_buffer(void);
void free_prompt(void);

#endif // UI_H
