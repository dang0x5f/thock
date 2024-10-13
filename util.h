#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SESSION_NOTASK,
    SESSION_RUN,
    SESSION_STATS,
    SESSION_MODULES,
    SESSION_END
} SessionTask;

typedef enum {
    SET_NOTASK,
    SET_RELOAD,
    SET_END
} SetTask;

typedef enum {
    WC_CURSOR,
    WC_CORRECT,
    WC_INCORRECT,
    WC_CHECKPOINT_ON,
    WC_CHECKPOINT_OFF,
    WC_OUT_OF_REACH,
    WC_NEWLINE
} WCState;

typedef struct {
    wchar_t* name;
    wchar_t* (*get_wordset)(uint32_t*);
} Module;

extern Module modules[];

bool initialize_program(void);
bool initialize_interface(void);

SessionTask retrieve_session_task(void);
SetTask retrieve_set_task(void);

void thock(void);
void draw(void);

void exit_program(void);

#endif // UTIL_H
