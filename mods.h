#include "config.h"

#ifdef WITH_MODS
#include "dictmod.h"
#endif

Module modules[] = {

    { "DefaultText"
    , get_default_wordset }
    ,

#ifdef WITH_MODS
    { "DictionaryMod"
    , get_random_wordset }
#endif

};
