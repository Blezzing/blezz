#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include"file.h"
#include"errors.h"
#include"argpass.h"
#include"data.h"

int configLines;
int contentLines;

char** getLines(FILE* file) {
    int lines_to_allocate = 64;
    int line_size_to_allocate = 100;
    
    if (file==NULL) {
        fileError();
    }

    char** lines = (char**)malloc(sizeof(char*)*lines_to_allocate);
    if (lines==NULL) {
        memError();
    }
    
    int i;
    for (i = 0; 1; i++) {
        //do we need to allocate more lines?
        if (i >= lines_to_allocate) {
            lines_to_allocate = lines_to_allocate * 2;
            lines = (char**)realloc(lines,sizeof(char*)*lines_to_allocate);
            if (lines==NULL) {
                memError();
            }
        }
        
        //allocate line, and make sure we succeed
        lines[i] = malloc(line_size_to_allocate);
        if (lines[i]==NULL) {
            memError();
        }
        
        //break if done
        if (fgets(lines[i],line_size_to_allocate-1,file)==NULL) {
            break;
        }
        
        //remove newline
        int j;
        for (j = strlen(lines[i])-1; j >= 0 && (lines[i][j] == '\n' || lines[i][j] == '\r'); j--) {;}
        lines[i][j+1]='\0';
    }
    contentLines = i+1;

    //Set line after last to null for future loop terminations
    lines[contentLines] = NULL;
    
    return lines;   
}

void importContent(char* path) {
    FILE* file;
    char** lines;
    Dir* root = NULL;

    printf("%s %s\n","Loading content from:",path); 

    if (access(path, R_OK) == -1) {
        printf("\tNo content file exists, showing error.\n");
        startDir = newDirFromDecl("Nothing to show");
        return;
    }
    
    file = fopen(path,"r"); //add errorhandling here, missing folders give segfault
    lines = getLines(file);
    fclose(file);
    
    Dir* currDir = NULL;
    for (int i = 0; lines[i] != NULL; i++) {
        if (lines[i][0] == '#') {//it's a comment, do not consider following possibilities
        }

        else if (isActRef(lines[i])) {
            Act* act = newActFromRef(lines[i],currDir);
            addAct(act);
        } 

        else if (isDirDecl(lines[i])) {
            Dir* dir = newDirFromDecl(lines[i]);
            dir = addDir(dir); //dir might be replaced with an already existing dir
            currDir = dir;
            if(root==NULL) {
                root = dir;
            }
        }

        else if (isDirRef(lines[i])) {
            Dir* dir = newDirFromRef(lines[i],currDir);
            addDir(dir);
        }   

        free(lines[i]);    
    }
    free(lines);

    printf("\tLoaded %i directories\n\tLoaded %i actions\n",savedDirs, savedActs);

	startDir = root;
}

int startsWithString(const char* check, const char* string) {
    int cLen = strlen(check);
    int sLen = strlen(string);
    if (cLen >= sLen) {
        return (strncmp(check,string,sLen))?0:1;
    } else {
        return 0;
    }
}

void assignConfigString(char** target, const char* source, const char* filter) {
    int sizeSource = strlen(source);
    int sizeFilter = strlen(filter);
    int sizeNew = sizeSource-sizeFilter;

    *target = (char*)malloc(sizeNew*sizeof(char));
    memcpy(*target,source+sizeFilter,sizeNew);
    (*target)[sizeNew] = '\0';
}

void assignConfigChar(char* target, const char* source, const char* filter) {
    int sizeFilter = strlen(filter);

    *target = source[sizeFilter];
}

void assignConfigInt(int* target, const char* source, const char* filter) {
    int sizeFilter = strlen(filter);

    *target = (int)strtol(source+sizeFilter,NULL,10);
}

void assignConfigHex(int* target, const char* source, const char* filter) {
    int sizeFilter = strlen(filter);

    *target = (int)strtol(source+sizeFilter,NULL,16);
}

void assignConfigBool(int* target, const char* source, const char* filter) {
    int sizeFilter = strlen(filter);

    *target = ((int)strtol(source+sizeFilter,NULL,10))?1:0;
}

void assignConfigXPos(xPos_enum_t* target, const char* source, const char* filter) {
    int sizeFilter = strlen(filter);

    switch(source[sizeFilter]) {
        case ('r'): *target=XRight; break;
        case ('m'): *target=XMid; break;
        case ('l'): *target=XLeft; break;
    }
}

void assignConfigYPos(yPos_enum_t* target, const char* source, const char* filter) {
    int sizeFilter = strlen(filter);

    switch(source[sizeFilter]) {
        case ('t'): *target=YTop; break;
        case ('m'): *target=YMid; break;
        case ('b'): *target=YBot; break;
    }
}

void importConfig(char* path) {
    FILE* file;
    char** lines;

    printf("%s %s\n","Loading config from:",path);

    if (access(path, R_OK) == -1) {
        printf("\tNo config file exists, using defaults.\n");
        return;
    }
 
    file = fopen(path,"r");
    lines = getLines(file);
    fclose(file);

    //config variables we accept
    static const char* fontString                = "font=";
    static const char* dirUpString               = "directoryUpKey=";
    static const char* actionIndicatorString     = "actionIndicator=";
    static const char* directoryIndicatorString  = "directoryIndicator=";
    static const char* startDirectoryString      = "startDirectory=";
    static const char* windowWidthString         = "windowWidth=";
    static const char* showKeyAsUpperString      = "showKeyAsUpper=";
    static const char* showMenuNamesString       = "showMenuNames=";
    static const char* showMenuNamesNestedString = "showMenuNamesNested=";
    static const char* foregroundColorString     = "foregroundColor=";
    static const char* backgroundColorString     = "backgroundColor=";
    static const char* topIndentString           = "topIndentation=";
    static const char* leftIndentString          = "leftIndentation=";
    static const char* bottomIndentString        = "bottomIndentation=";
    static const char* windowXOffsetString       = "windowXOffset=";
    static const char* windowYOffsetString       = "windowYOffset=";
    static const char* windowXPosString          = "windowXPosition=";
    static const char* windowYPosString          = "windowYPosition=";

    for (int i = 0; lines[i] != NULL; i++) {
        if (startsWithString(lines[i],fontString)) {
            assignConfigString(&(arguments.font),lines[i],fontString);
            printf("\tLoaded %.*s as: %s\n",(int)strlen(fontString)-1,fontString,arguments.font);
        } 
        else if (startsWithString(lines[i],dirUpString)) {
            assignConfigChar(&(arguments.dirUpKey),lines[i],dirUpString);
            printf("\tLoaded %.*s as: %c\n",(int)strlen(dirUpString)-1,dirUpString,arguments.dirUpKey);
        
        }
        else if (startsWithString(lines[i],actionIndicatorString)) {
            assignConfigChar(&(arguments.actS),lines[i],actionIndicatorString);
            printf("\tLoaded %.*s as: %c\n",(int)strlen(actionIndicatorString)-1,actionIndicatorString,arguments.actS);
        
        }
        else if (startsWithString(lines[i],directoryIndicatorString)) {
            assignConfigChar(&(arguments.dirS),lines[i],directoryIndicatorString);
            printf("\tLoaded %.*s as: %c\n",(int)strlen(directoryIndicatorString)-1,directoryIndicatorString,arguments.dirS);
        }
        else if (startsWithString(lines[i],startDirectoryString)) {
            assignConfigString(&(arguments.startDir),lines[i],startDirectoryString);
            printf("\tLoaded %.*s as: %s\n",(int)strlen(startDirectoryString)-1,startDirectoryString,arguments.startDir);
        }
        else if (startsWithString(lines[i],windowWidthString)) {
            assignConfigInt(&(arguments.windowWidth),lines[i],windowWidthString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(windowWidthString)-1,windowWidthString,arguments.windowWidth);
        }
        else if (startsWithString(lines[i],showKeyAsUpperString)) {
            assignConfigBool(&(arguments.keyAsUpper),lines[i],showKeyAsUpperString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(showKeyAsUpperString)-1,showKeyAsUpperString,arguments.keyAsUpper);
        }
        else if (startsWithString(lines[i],showMenuNamesString)) {
            assignConfigBool(&(arguments.showMenuNames),lines[i],showMenuNamesString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(showMenuNamesString)-1,showMenuNamesString,arguments.showMenuNames);
        }
        else if (startsWithString(lines[i],showMenuNamesNestedString)) {
            assignConfigBool(&(arguments.showMenuNamesNested),lines[i],showMenuNamesNestedString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(showMenuNamesNestedString)-1,showMenuNamesNestedString,arguments.showMenuNamesNested);
        }
        else if (startsWithString(lines[i],foregroundColorString)) {
            assignConfigHex(&(arguments.fgColor),lines[i],foregroundColorString);
            printf("\tLoaded %.*s as: 0x%6.x\n",(int)strlen(foregroundColorString)-1,foregroundColorString,arguments.fgColor);
        }
        else if (startsWithString(lines[i],backgroundColorString)) {
            assignConfigHex(&(arguments.bgColor),lines[i],backgroundColorString);
            printf("\tLoaded %.*s as: 0x%6.x\n",(int)strlen(backgroundColorString)-1,backgroundColorString,arguments.bgColor);
        }
        else if (startsWithString(lines[i],topIndentString)) {
            assignConfigInt(&(arguments.topIndent),lines[i],topIndentString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(topIndentString)-1,topIndentString,arguments.topIndent);
        }
        else if (startsWithString(lines[i],leftIndentString)) {
            assignConfigInt(&(arguments.leftIndent),lines[i],leftIndentString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(leftIndentString)-1,leftIndentString,arguments.leftIndent);
        }
        else if (startsWithString(lines[i],bottomIndentString)) {
            assignConfigInt(&(arguments.botIndent),lines[i],bottomIndentString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(bottomIndentString)-1,bottomIndentString,arguments.botIndent);
        }
        else if (startsWithString(lines[i],windowXOffsetString)) {
            assignConfigInt(&(arguments.winXOffset),lines[i],windowXOffsetString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(windowXOffsetString)-1,windowXOffsetString,arguments.winXOffset);
        }
        else if (startsWithString(lines[i],windowYOffsetString)) {
            assignConfigInt(&(arguments.winYOffset),lines[i],windowYOffsetString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(windowYOffsetString)-1,windowYOffsetString,arguments.winYOffset);
        }
        else if (startsWithString(lines[i],windowXPosString)) {
            assignConfigXPos(&(arguments.winXPos),lines[i],windowXPosString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(windowXPosString)-1,windowXPosString,arguments.winXPos);
        }
        else if (startsWithString(lines[i],windowYPosString)) {
            assignConfigYPos(&(arguments.winYPos),lines[i],windowYPosString);
            printf("\tLoaded %.*s as: %i\n",(int)strlen(windowYPosString)-1,windowYPosString,arguments.winYPos);
        }

        free(lines[i]);
    }
    free(lines);
}