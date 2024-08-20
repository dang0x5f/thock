#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

//"/usr/share/misc/iso3166",

const int filecount = 4;
const char* filepaths[] = {
    "/usr/share/dict/web2",
    "/usr/share/dict/web2a",
    "/usr/share/dict/freebsd",
    "/usr/share/dict/propernames"
};

void openfiles(FILE**);
void assessfiles(FILE**,int*);
void randomwords(FILE**,int*,char*,int);
void closefiles(FILE**);

int main(void){
    FILE* files[filecount];    
    int file_lengths[filecount];
    char bunch_o_words[1024];

    openfiles(files);

    assessfiles(files, file_lengths);

    randomwords(files, file_lengths, bunch_o_words, 1024);

    closefiles(files);
    return(0);
}

void openfiles(FILE** files){
    for(int x = 0; x < filecount; x++)
        assert( (files[x] = fopen(filepaths[x],"r")) != NULL);
}

void assessfiles(FILE** files, int* file_lengths){
    int linecount;
    char buffer[256];

    for(int x = 0; x < filecount; x++){
        linecount = 0;
        while(!feof(files[x])){
            fgets(buffer, sizeof(buffer), files[x]);
            linecount++;
        }
        file_lengths[x] = linecount;
        /* printf("Lines: %d\n", file_lengths[x]); */
        rewind(files[x]);
    }
}

void randomwords(FILE** files, int* file_lengths, char* buffer, int bufsize){
    int randomline, offset;

    srand(time(NULL));
    for(int j = 0; j < 5; j++){
        for(int i = 0; i < filecount; i++){

            offset = 0;
            randomline = ( (int)(( (double)rand()/RAND_MAX)*file_lengths[i]) );

            for(int x = 1; x < randomline; x++)
                offset += strlen(fgets(buffer, bufsize, files[i]));

            fseek(files[i], offset, SEEK_SET);
            printf("%s", fgets(buffer,bufsize,files[i]));
            /* fgets(buffer,bufsize,files[i]); */
            /* buffer[strlen(buffer) - 1] = ' '; */
            /* printf("%s", buffer); */
            rewind(files[i]);
        }
    }
}

void closefiles(FILE** files){
    for(int x = 0; x < filecount; x++)
        fclose(files[x]);
}
