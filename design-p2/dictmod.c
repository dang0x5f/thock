#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "dictmod.h"

char* get_random_wordset(int* size)
{
    FILE* files[FILECOUNT];    
    int file_lengths[FILECOUNT];
    char* wordset = NULL;

    open_files(files);

    assess_files(files, file_lengths);

    wordset = generate_words(files, file_lengths, size);

    close_files(files);

    return(wordset);
}

void open_files(FILE** files)
{
    /* TODO: maybe handle more gracefully than assert */ 
    for(int x = 0; x < FILECOUNT; x++)
        assert( (files[x] = fopen(filepaths[x],"r")) != NULL);
}

void assess_files(FILE** files, int* file_lengths)
{
    const int BUFFER_SIZE = 256;
    int linecount;
    char buffer[BUFFER_SIZE];   // TODO: does buffer size need to be this long?

    for(int x = 0; x < FILECOUNT; x++){
        linecount = 0;
        while(!feof(files[x])){
            fgets(buffer,BUFFER_SIZE,files[x]);
            linecount++;
        }
        file_lengths[x] = linecount;
        rewind(files[x]);
    }
}

char* generate_words(FILE** files, int* file_lengths, int* size)
{
    const int BUFFER_SIZE = 256;
    char* wordset = NULL;
    char buffer[BUFFER_SIZE];   // TODO: does buffer size need to be this long?
    int randomline, offset;

    srand(getpid());
    for(int loop = 0; loop < LOOPS; loop++){
        for(int file = 0; file < FILECOUNT; file++){

            offset = 0;
            randomline = ((int)(((double)rand()/RAND_MAX)*file_lengths[file]));
            
            for(int line = 1; line < randomline; line++)
                offset += strlen(fgets(buffer,BUFFER_SIZE,files[file]));

            fseek(files[file], offset, SEEK_SET);
            fgets(buffer, BUFFER_SIZE, files[file]);

            buffer[strlen(buffer)-1] = ' ';

            /* wordset malloc/realloc */
            if(wordset == NULL){
                wordset = calloc(strlen(buffer)+1,sizeof(char));
                strcpy(wordset,buffer);
            }else{
                char* temp_wordset = calloc(strlen(wordset)+1,sizeof(char));
                strcpy(temp_wordset,wordset);
                wordset = realloc(wordset,sizeof(char) * (strlen(wordset)+strlen(buffer)+1));
                strcpy(wordset,temp_wordset);
                strcat(wordset,buffer);
                free(temp_wordset);
            }
            rewind(files[file]);
        }
    }

    /* TODO: realloc once more -1 to get rid of space ; may keep after all */
    
    *size = strlen(wordset) + 1;
    return(wordset);
}

/* TODO: delete later, probably wont need */
/* void format_words(char**,int,int) */
/* { */
    
/* } */

/* void free_words(char**,int) */
/* { */

/* } */

void close_files(FILE** files)
{
    for(int x = 0; x < FILECOUNT; x++)
        fclose(files[x]);
}
