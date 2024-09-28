#ifndef THOCK_H
#define THOCK_H

/* /1* TODO: delete later *1/ */
/* char* bar(int width){ */
/*     return "barfoo"; */
/* } */
/* /1* TODO: delete later *1/ */
/* char* foo(int width){ */
/*     return "foobar"; */
/* } */

/* /1* TODO: delete later *1/ */
/* DataSource dict = { */
/*     "foo", */
/*     bar */
/* }; */
/* /1* TODO: delete later *1/ */
/* DataSource dict1 = { */
/*     "bar", */
/*     foo */
/* }; */

typedef struct {
    char* name;
    char* (*get_wordset)(int*);
} DataModule;

/* extern DataModule data_modules[]; */

#endif // THOCK_H
