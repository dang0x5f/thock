#ifndef THOCK_H
#define THOCK_H

typedef struct {
    wchar_t* name;
    wchar_t* (*get_wordset)(int*);
} Module;

extern Module modules[];

#endif // THOCK_H
