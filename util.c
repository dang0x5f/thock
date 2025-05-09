#include <stdio.h>
#include <locale.h>
#include <assert.h>

#define ctrl(x)             ( (x) & 0x1F )

#include "ui.h"
#include "util.h"
#include "mods.h"

static ProgramState program_state;
static ProgramState shelfed_state;
static int fail_index = FI_OFF;

bool initialize_program(void)
{
    /* setlocale(LC_ALL,""); */

    uint8_t mod_count = sizeof(modules)/sizeof(Module);

    if(!mod_count){
        fprintf(stderr,"no modules\n");
        return(false);
    }

    set_ps(PS_OUTSET);

    return(true);
}

bool initialize_interface(void)
{
    if(!initialize_stdscr()){
        fprintf(stderr,"initialize_stdscr() failed\n");
        return(false);
    }

    if(!initialize_wordset()){
        fprintf(stderr,"initialize_wordset() failed\n");
        return(false);
    }

    if(!initialize_textview()){
        fprintf(stderr,"initialize_textview() failed\n");
        return(false);
    }

    if(!initialize_prompt()){
        fprintf(stderr,"initialize_prompt() failed\n");
        return(false);
    }

    if(!initialize_stats()){
        fprintf(stderr,"initialize_stats() failed\n");
        return(false);
    }

    return(true);
}

void set_ps(ProgramState ps)
{
    program_state = ps;
}

ProgramState get_ps(void)
{
    return program_state;
}

void save_ps(ProgramState ps)
{
    shelfed_state = ps;
}

void restore_ps(void)
{
    program_state = shelfed_state;
}

void set_fi(int index)
{
    fail_index = index;
}

int get_fi(void)
{
    return(fail_index);
}

SessionTask retrieve_session_task(void)
{
    SessionTask task = SESSION_NOTASK;

    int keycode = get_keycode();
    switch(keycode){
        case ctrl('g'):
            if(!too_small())
                task = SESSION_RUN;
            break;
        case ctrl('x'):
            task = SESSION_END;
            break;
    }

    return(task);
}

void thock(void)
{
    bool end_set = false;
    SetTask task = SET_NOTASK;

    set_ps(PS_INSET);
    draw_textview();
    toggle_cursor(CVS_NORMAL);
    place_cursor();

    while(!end_set){
        task = retrieve_set_task();
        switch(task){
            case SET_NOTASK:
                break;
            case SET_RELOAD:
                /* TODO refactor */
                initialize_stdscr();
                initialize_stats();
                initialize_textview();
                initialize_wordset_state();
                initialize_wordset_segments();
                initialize_prompt();
                toggle_cursor(CVS_NORMAL);
                reset_prompt();
                draw();
                /* if(!too_small()) */
                /*     reload(); */
                break;
            case SET_END:
                end_set = true;
                break;
        }
    }
    toggle_cursor(CVS_INVISIBLE);
    set_ps(PS_OUTSET);
    draw_textview();
    /* return(???); */
}

SetTask retrieve_set_task(void)
{
    bool set_active = true;
    bool set_completed = false;
    SetTask task = SET_NOTASK;
    int keycode;

    while(set_active){
        keycode = get_keycode();
        switch(keycode){
            case ctrl('r'):
                task = SET_RELOAD;
                set_active = false;
                break;
            case ctrl('x'):
                task = SET_END;
                set_active = false;
                set_completed = true;
                break;
            default:
                set_completed = use_keycode(keycode);
                break;
        }

        if(set_completed){
            task = SET_END;
            set_active = false;
            initialize_wordset_state();
            initialize_wordset_segments();
            reset_prompt();
        }
    }

    return(task);
}

/* TODO: bit field to decide which windows to draw */
void draw(void)
{
    draw_stdscr();
    draw_textview();
    draw_prompt();
}

void exit_program(void)
{
    free_wordset_wctext();
    free_textview();
    free_buffer();
    free_prompt();
    free_stdscr_exit();
}
