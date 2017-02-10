#include<stdlib.h>
#include<string.h>

#include"file.h"
#include"errors.h"
#include"argpass.h"
#include"data.h"

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
        
        //get line and verify, that we aren't done
        if (fgets(lines[i],line_size_to_allocate-1,file)==NULL) {
            break;
        }
        
        //get rid of CR/LF at EOL
        int j;
        for (j = strlen(lines[i])-1; j >= 0 && (lines[i][j] == '\n' || lines[i][j] == '\r'); j--)
            ;
        lines[i][j+1]='\0';
    }
    contentLines = i+1;
    
    return lines;   
}

void importContent(char* path) {
    FILE* file;
    char** lines;
    Dir* root = NULL;
    
    printf("%s %s\n","Loading content from:",path); 
    file = fopen(path,"r"); //TODO: add errorhandling here, missing folders give segfault
    lines = getLines(file);
    fclose(file);
    
    Dir* currDir = NULL;
    for (int i = 0; lines[i] != NULL; i++) {
        if (lines[i][0] == '#') {//it's a comment, do not consider following possibilities
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

        else if (isActRef(lines[i])) {
            Act* act = newActFromRef(lines[i],currDir);
            addAct(act);
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

void importConfig(char* path) {
    FILE* file;
    char** lines;

    printf("%s %s\n","Loading config from:",path); 
    file = fopen(path,"r");
    lines = getLines(file);
    fclose(file);

    //config variables we accept
    const char* fontString = "font=";
    const char* dirUpString = "directoryUpKey=";
    const char* actionIndicatorString = "actionIndicator=";
    const char* directoryIndicatorString = "directoryIndicator=";
    const char* startDirectoryString = "startDirectory=";
    const char* windowWidthString = "windowWidth=";

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

        free(lines[i]);
    }
    free(lines);
}