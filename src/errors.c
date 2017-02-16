#include"errors.h"
#include"mutex.h"

#include<stdio.h>
#include<stdlib.h>

void exitWithError(int errorCode){
    applicationRelease();
    exit(errorCode);
}

void memError(){
    fprintf(stderr,"Out of memory.\n");
    exitWithError(-1);
}

void fileError(){
    fprintf(stderr,"Error opening file.\n");
    exitWithError(-2);
}

void guiError(char* message) {
    fprintf(stderr,"Error in gui, got the message: %s\n",message);
    exitWithError(-3);
}