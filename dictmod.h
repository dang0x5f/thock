#ifndef DICTMOD_H
#define DICTMOD_H

#include <stdio.h>

#define LOOPS    10
#define FILECOUNT 4

static const char* filepaths[] = {
    "/usr/share/dict/web2",
    "/usr/share/dict/web2a",
    "/usr/share/dict/freebsd",
    "/usr/share/dict/propernames"
};

char* get_random_wordset(int*);

void open_files(FILE**);
void assess_files(FILE**,int*);
char* generate_words(FILE**,int*,int*);
/* void format_words(char**,int,int); */
/* void free_words(char**,int); */
void close_files(FILE**);

#endif // DICTMOD_H
