#ifndef FILEHEADER_H
#define FILEHEADER_H

#include<stdio.h>
#include"types.h"

int configLines;

char** getLines(FILE* file);
Dir* importData(char* path);

#endif