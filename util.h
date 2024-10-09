#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

enum char_status {
    CURSOR,
    EMPTY,
    CORRECT,
    INCORRECT,
    LOCKED,
    UNLOCKED
};

typedef struct {
    wchar_t* name;
    wchar_t* (*get_wordset)(int*);
} Module;

extern Module modules[];

void request_wordset(uint8_t*,uint32_t*);
void init_wordset_state(wchar_t*,uint8_t*,uint32_t);

#endif // UTIL_H
