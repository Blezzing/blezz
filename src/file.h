#ifndef FILEHEADER_H
#define FILEHEADER_H

#include<stdio.h>

extern int configLines;
extern int contentLines;

void importConfig(char* path);
void importContent(char* path);

#endif