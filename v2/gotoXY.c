#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


void gotoXY(int x, int y){
    printf("\033[%d;%df]", y, x);
}

int main(){
    gotoXY(10,10);
}

