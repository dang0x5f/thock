#ifndef DICTMOD_H
#define DICTMOD_H

#include <stdio.h>
#include <wchar.h>

#define LOOPS    10
#define FILECOUNT 4

static const char* filepaths[] = {
    "/usr/share/dict/web2",
    "/usr/share/dict/web2a",
    "/usr/share/dict/freebsd",
    "/usr/share/dict/propernames"
};

wchar_t* get_random_wordset(uint32_t*);

void open_files(FILE**);
void assess_files(FILE**,int*);
wchar_t* generate_words(FILE**,int*,uint32_t*);
/* void format_words(char**,int,int); */
/* void free_words(char**,int); */
void close_files(FILE**);

#endif // DICTMOD_H
