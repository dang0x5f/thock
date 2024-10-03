#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <string.h>

/* static char* sample_text = "one one one one one one one one one one one one\none one one one one one one one one one one one\none one one one one one one one one one one one"; */

static const char* default_text = "And reading itself is an amazing activity: You\nglance at a thin, flat object made from a\ntree...and the voice of the author begins to\nspeak inside your head. (Hello!) ";

static int newlines = 2;

static char* get_default_wordset(int* size){
    char* text;

    *size = strlen(text);
    text = malloc( ((*size)+1) * sizeof(char) );
    strcpy(text,default_text);
    
    return(text);
}

#endif // CONFIG_H
