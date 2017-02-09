#ifndef FILEHEADER_H
#define FILEHEADER_H

#include<stdio.h>

int configLines;
int contentLines;

void importConfig(char* path);
void importContent(char* path);

#endif