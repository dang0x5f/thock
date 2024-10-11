#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static const wchar_t* default_text = L"And reading itself is an amazing activity: You\nglance at a thin, flat object made from a\ntree...and the voice of the author begins to\nspeak inside your head. (Hello!) ";
/* static const wchar_t* default_text = L"привет"; */

static wchar_t* get_default_wordset(uint32_t* size){
    wchar_t* text = NULL;

    *size = wcslen(default_text);
    text = malloc( ((*size)+1) * sizeof(wchar_t) );
    wcscpy(text,default_text);
    
    return(text);
}

#endif // CONFIG_H
