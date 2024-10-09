#include "config.h"

#ifdef WITH_MODS
#include "dictmod.h"
#endif

Module modules[] = {

    { L"DefaultText" , get_default_wordset }
    ,

#ifdef WITH_MODS
    { L"DictionaryMod" , get_random_wordset }
    ,
#endif

};
