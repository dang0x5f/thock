#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SESSION_RUN,
    SESSION_STATS,
    SESSION_MODULES,
    SESSION_END
} task_t;

typedef enum {
    WC_CURSOR,
    WC_CORRECT,
    WC_INCORRECT,
    WC_CHECKPOINT_ON,
    WC_CHECKPOINT_OFF,
    WC_OUT_OF_REACH,
    WC_NEWLINE
} wcstate_t;

typedef struct {
    wchar_t* name;
    wchar_t* (*get_wordset)(uint32_t*);
} Module;

extern Module modules[];

bool initialize_program(void);
bool initialize_interface(void);


#endif // UTIL_H
