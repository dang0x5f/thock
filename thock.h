#ifndef THOCK_H
#define THOCK_H

typedef struct {
    char* name;
    char* (*get_wordset)(int*);
} Module;

extern Module modules[];

#endif // THOCK_H
