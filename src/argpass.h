#ifndef ARGPASSHEADER_H
#define ARGPASSHEADER_H

//structure for sharing informations about arguments
struct arguments {
    char *args[2];
    int silent, verbose, windowWidth, keyAsUpper, showMenuNames, showMenuNamesNested, fgColor, bgColor, topIndent, leftIndent, botIndent, winXOffset, winYOffset;
    char dirUpKey, actS, dirS;
    char *configFile, *contentFile, *font, *startDir;
};

struct arguments arguments;

void argumentsInit();
void argumentsApply(int argc,char** argv);

#endif