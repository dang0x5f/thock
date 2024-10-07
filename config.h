#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <string.h>

static const char* default_text = "And reading itself is an amazing activity: You\nglance at a thin, flat object made from a\ntree...and the voice of the author begins to\nspeak inside your head. (Hello!) ";

static char* get_default_wordset(int* size){
    char* text;

    *size = strlen(default_text);
    text = malloc( ((*size)+1) * sizeof(char) );
    strcpy(text,default_text);
    
    return(text);
}

#endif // CONFIG_H
