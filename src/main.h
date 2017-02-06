#ifndef BLEZZHEADER_H
#define BLEZZHEADER_H

int configLines;
char** allocForDirToStrings();
void dirToStrings(char** ret, int* count);
int selectElement(char choice);
#endif