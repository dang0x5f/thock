#include <ncurses.h>
#include <assert.h>
#include <string.h>

/* # THOCK_H
 *
 * char* wordset/char* text
 * char text_state[text.len]
 * size_t wordset_len
 * enum states { CURS, CORRECT, ... }
 * int fail_state
 * int fail_index
 *
 * freewordset()
 * init_segments()
 * check_segments()
 * set_new_segments()
 * init_text_state()
 * set_text_state()
 */

/* # DICTDS_H
 *
 * char files[] = { "path/file", ... }
 * size_t filecount
 *
 * openfiles()
 * assessfile()
 * randomwords()
 * closefile()
 */

/* # NCUI_H
 * 
 * int newlines
 * int std_x, std_y
 * int h_offset
 *
 * define MAXWIDTH 
 * define MINWIDTH 
 * define KEY_SPACE 32
 * define KEY_ESCAPE 27
 *
 * WINDOW prompt
 * int pw_x, pw_y, pw_height
 * int promptrows, promptcols
 *
 * WINDOW textwin
 * int tw_x, tw_y, tw_height
 *
 * char buffer[pcols-2]
 * int buffer_index
 *
 * reset_stdscr()
 * clear_prompt_buffer()
 * write_text()
 * write_char()
 * resize_layout()
 * scroll_up()
 * scroll_down()
 * 
 */

int main(){
    

    return(0);
}
