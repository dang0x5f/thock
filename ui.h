#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <stdint.h>

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
bool initialize_stats(void);

void sanitize_nl(void);
void insert_nl(void);

    void reset_wordset(void);
void reset_prompt(void);
void reset_buffer(void);

void draw_stdscr(void);
void draw_textview(void);
void draw_prompt(void);
void draw_colorscheme(void);

bool compare_segments(void);
void do_resize(void);
bool too_small(void);

int get_keycode(void);
bool use_keycode(int);

void scroll_down(void);

void backspace_buffer(void);
void update_buffer(int);
bool update_state(int*);
void update_segments(void);
void write_textview_wordset_wctext(void);
void write_textview_wordset_wcextended(void);
void write_prompt(void);

bool convert_to_wordset_wcextended(void);

void place_cursor(void);
void toggle_cursor(CursorVisibilityState);
void* run_timer(void*);

void free_stdscr_resize(void);
void free_stdscr_exit(void);
void free_wordset_wctext(void);
void free_textview(void);
void free_buffer(void);
void free_prompt(void);

#endif // UI_H
