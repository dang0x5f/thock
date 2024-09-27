#ifndef THOCK_H
#define THOCK_H

/* TODO: delete later */
char* bar(int width){
    return "foobar";
}

typedef struct {
    char* name;
    char* (*get_wordset)(int);
} DataSource;

/* TODO: delete later */
DataSource dict = {
    "foo",
    bar
};

#endif // THOCK_H
