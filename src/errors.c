#include"errors.h"

#include<stdio.h>
#include<stdlib.h>

void memError(){
    fprintf(stderr,"Out of memory.\n");
    exit(-1);
}

void fileError(){
    fprintf(stderr,"Error opening file.\n");
    exit(-2);
}

void guiError(char* message) {
    fprintf(stderr,"Error in gui, got the message: %s\n",message);
    exit(-3);
}