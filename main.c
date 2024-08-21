#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

//"/usr/share/misc/iso3166",

const int loops         = 5;
const int filecount     = 4;
const char* filepaths[] = {
    "/usr/share/dict/web2",
    "/usr/share/dict/web2a",
    "/usr/share/dict/freebsd",
    "/usr/share/dict/propernames"
};

void openfiles(FILE**);
void assessfiles(FILE**,int*);
void randomwords(FILE**,int*,char**,int);
void freebunch(char**,int);
void closefiles(FILE**);

int main(void){
    FILE* files[filecount];    
    int file_lengths[filecount];
    char* wordset[loops*filecount];

    openfiles(files);

    assessfiles(files,file_lengths);

    randomwords(files,file_lengths,wordset,loops);

    int totalbytes = 0;
    for(int x = 0; x < loops*filecount; x++){
        printf("%s,", wordset[x]);
        totalbytes += strlen(wordset[x]);
    }
    printf("\n%d\n", totalbytes);
    printf("\n%lu\n", sizeof(wordset));

    freebunch(wordset,loops*filecount);
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

void randomwords(FILE** files, int* file_lengths, char** words, int loops){
    char buffer[256];
    int randomline, offset, index = 0;

    srand(time(NULL));
    for(int j = 0; j < loops; j++){
        for(int i = 0; i < filecount; i++){

            offset = 0;
            randomline = ( (int)(( (double)rand()/RAND_MAX)*file_lengths[i]) );

            for(int x = 1; x < randomline; x++)
                offset += strlen(fgets(buffer,256,files[i]));


            fseek(files[i], offset, SEEK_SET);
            fgets(buffer,256,files[i]);
            buffer[strlen(buffer) - 1] = '\0';
            words[index] = calloc(strlen(buffer)+1,sizeof(char));
            strcpy(words[index],buffer);

            /* printf("%s", buffer); */

            rewind(files[i]);
            index++;
        }
    }
}

void freebunch(char** words, int len){
    for(int x = 0; x < len; x++){
        free(words[x]);
    }
}

void closefiles(FILE** files){
    for(int x = 0; x < filecount; x++)
        fclose(files[x]);
}
