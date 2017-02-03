#ifndef FILEHEADER_H
#define FILEHEADER_H

#include"types.h"

char** getLines(FILE* file);
Dir* importData(char* path);

#endif