#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ctype.h>

#include"file.h"
#include"data.h"
#include"argpass.h"

int savedDirs = 0;
int savedActs = 0;
Dir** allDirs = NULL;
Act** allActs = NULL;
int dirStackTop = -1;
Dir** dirStack;
Dir* startDir;

int isDirRef(char* string){
    return strlen(string)>6 && (strncmp(string,"dir(",4) == 0);
}

int isActRef(char* string){
    return strlen(string)>8 && (strncmp(string,"act(",4) == 0);
}

int isDirDecl(char* string){
    return (strcspn(string,":") != strlen(string));
}

Dir* newDirFromRef(char* string, Dir* parent){
    int keyIndex = 4;
    int labelIndex = 6;
    int labelLength = strlen(string)-labelIndex-1;
    
    Dir* dir = (Dir*)malloc(sizeof(Dir));
    dir->key = tolower(string[keyIndex]);
    dir->label = (char*)malloc(sizeof(char)*(labelLength+1));
    strncpy(dir->label,string+labelIndex,labelLength);
    dir->label[labelLength]='\0';
    dir->parent = parent;
    
    return dir;
}

Dir* newDirFromDecl(char* string){
    int colonIndex = strcspn(string,":");
    
    Dir* dir = (Dir*)malloc(sizeof(Dir));
    dir->key = '\0';
    dir->label = (char*)malloc(sizeof(char)*(colonIndex+1));
    strncpy(dir->label,string,colonIndex);
    dir->label[colonIndex]= '\0';
    dir->parent = NULL;
    
    return dir;
}

Act* newActFromRef(char* string, Dir* parent){
    int keyIndex = 4;
    int labelIndex = 6;
    int labelLength = strcspn(string+labelIndex,",");
    int commandIndex = labelIndex + labelLength+1;
    int commandLength = strcspn(string+commandIndex,")");
    
    char* disownString = "&disown";
    int disownStringLength = 8;

    Act* act = (Act*)malloc(sizeof(Act));
    act->key = tolower(string[keyIndex]);
    act->label = (char*)malloc(sizeof(char)*(labelLength+1));
    strncpy(act->label,string+labelIndex,labelLength);
    act->label[labelLength] = '\0';
    act->command = (char*)malloc(sizeof(char)*(commandLength+1+disownStringLength));
    strncpy(act->command,string+commandIndex,commandLength);
    strncpy(act->command+commandLength,disownString,disownStringLength);

    act->command[commandLength+disownStringLength] = '\0';
    act->parent = parent;

    return act;
}

Dir* addDir(Dir* dir){
    if (allDirs==NULL) {
        allDirs = (Dir**)malloc(sizeof(Dir*)*contentLines);
    }
    
    for (int d = 0; d < savedDirs; d++) {
        if (strcmp(allDirs[d]->label,dir->label) == 0) {
            if (allDirs[d]->parent == NULL) {
                allDirs[d]->parent = dir->parent;
            }
            if (allDirs[d]->key == '\0') {
                allDirs[d]->key = dir->key;
            }
            free(dir->label);
            free(dir);
            
            return allDirs[d];
        }
    }
    
    return allDirs[savedDirs++] = dir;
}

Act* addAct(Act* act){
    if (allActs==NULL) {
        allActs = (Act**)malloc(sizeof(Act*)*contentLines);
    }
    
    return allActs[savedActs++] = act;
}

void dirStackPop() {
    dirStackTop--;
}

void dirStackPush(Dir* dir) {
    dirStack[++dirStackTop] = dir;
}

Dir* dirStackPeek() {
    return dirStack[dirStackTop];
}

int dirStackIsEmpty() {
    return (dirStackTop < 0);
}

void dirStackAlloc() {
    dirStack = (Dir**)malloc(sizeof(Dir*)*savedDirs);
}

//prepare array for storage of all strings to be printed
char** allocForDirToStrings() {
    char** ret = (char**)malloc(sizeof(char*)*(savedActs+savedDirs+1));
    for (int i = 0; i < savedActs+savedDirs+1; i++) {
        ret[i] = (char*)malloc(sizeof(char)*128);
    }
    return ret;
}

void stringifyCurrentMenuSimple(char** reference) {
	sprintf(*reference,"%s:",dirStackPeek()->label);
}

void stringifyCurrentMenuNested(char** reference) {
    char* divider = ">";
    int currentIndex = 0;

	sprintf(*reference,"%s:",dirStack[0]->label);
    currentIndex += strlen(dirStack[0]->label);
    for(int i = 1; i <= dirStackTop; i++) {
	    sprintf(*reference+currentIndex,"%s%s:",divider,dirStack[i]->label);
        currentIndex = strlen(*reference)-1;
    }
    (*reference)[currentIndex+1] = '\0';
}

//index parameter will tell how many lines are returned ASSUMING PRE ALLOCATED MEMORY
char** dirToStrings(char** ret, int* count) {
    int index = 0;

    if(arguments.showMenuNames) {
        if(arguments.showMenuNamesNested) {
            stringifyCurrentMenuNested(&(ret[index++]));
        }
        else {
            stringifyCurrentMenuSimple(&(ret[index++]));
        }
    }

    Dir* dir = dirStackPeek();

    for(int i = 0; i < savedDirs; i++) {
        if (allDirs[i]->parent == dir) {
            sprintf(ret[index++],"%c [%c] %s",arguments.dirS, (arguments.keyAsUpper?toupper(allDirs[i]->key):allDirs[i]->key), allDirs[i]->label);
        }
    }
    
    for(int i = 0; i < savedActs; i++) {
        if (allActs[i]->parent == dir) {
            sprintf(ret[index++],"%c [%c] %s",arguments.actS, (arguments.keyAsUpper?toupper(allActs[i]->key):allActs[i]->key), allActs[i]->label);
        }
    }  

    *count = index;
    return ret; //purely for readability
}

//selection in a dir, changes state if it should, if not returning 0 program should terminate.
int selectElement(char choice) {
    Dir* dir = dirStackPeek();
    //go dir up if dirUpKey
    if(choice == arguments.dirUpKey) {
        dirStackPop();
        if (dirStackIsEmpty()) {
            return ELEMENT_SELECTION_OVER;
        }
        else {
            return ELEMENT_SELECTION_TRUE;
        }
    }
    
    //go to dir if a match is found
    for(int i = 0; i < savedDirs; i++) {
        if(allDirs[i]->parent == dir && allDirs[i]->key == choice){
            dirStackPush(allDirs[i]);
            return ELEMENT_SELECTION_TRUE;
        }
    }
    
    //perform act if a match is found
    for(int i = 0; i < savedActs; i++) {
        if (allActs[i]->parent == dir && allActs[i]->key == choice){
            system(allActs[i]->command);
            return ELEMENT_SELECTION_OVER;
        }
    }
    
    return ELEMENT_SELECTION_FALSE;
}

Dir* findDirFromLabel(char* label) {    
    for(int i = 0; i < savedDirs; i++) {
        if(strcmp(allDirs[i]->label,label) == 0){
            return allDirs[i];
        }
    }

    return NULL;
}

void updateStartDir() {
    Dir* dir;
    if((dir = findDirFromLabel(arguments.startDir))) {
        startDir = dir;
    }
}

void dirStackInit() {
    dirStackAlloc();
    updateStartDir();
    dirStackPush(startDir);
}