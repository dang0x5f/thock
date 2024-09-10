#include <stdio.h>

void chn_ptr(int* num){
    *num = (*num) + 1;
}

int main(){
    int somenum = 10;

    printf("%d\n", somenum);
    chn_ptr(&somenum);
    printf("%d\n", somenum);

    return(0);
}
