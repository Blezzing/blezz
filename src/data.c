#include<stdlib.h>
#include<string.h>
#include"types.h"
#include"data.h"
#include"consts.h"

int configLines = 0;

int savedDirs = 0;
int savedActs = 0;
Dir** allDirs = NULL;
Act** allActs = NULL;

int dirStackTop = -1;
Dir** dirStack;

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
    int labelLength = strlen(string)-(labelIndex+1);
    
    Dir* dir = (Dir*)malloc(sizeof(Dir));
    dir->key = string[keyIndex];
    dir->label = (char*)malloc(sizeof(char)*(labelLength+1));
    strncpy(dir->label,string+labelIndex,labelLength);
    dir->label[labelLength]='\0';
    dir->parent = parent;
    
    return dir;
}

Dir* newDirFromDecl(char* string){
    int colonIndex = strcspn(string,":");
    
    Dir* dir = (Dir*)malloc(sizeof(Dir));
    dir->key = NO_KEY;
    dir->label = (char*)malloc(sizeof(char)*(colonIndex+1));
    strncpy(dir->label,string,colonIndex);
    dir->label[colonIndex]='\0';
    dir->parent = NULL;
    
    return dir;
}

Act* newActFromRef(char* string, Dir* parent){
    int keyIndex = 4;
    int labelIndex = 6;
    int labelLength = strcspn(string+labelIndex,",");
    int commandIndex = labelIndex + labelLength+1;
    int commandLength = strcspn(string+commandIndex,")");
    
    Act* act = (Act*)malloc(sizeof(Act));
    act->key = string[keyIndex];
    act->label = (char*)malloc(sizeof(char)*(labelLength+1));
    strncpy(act->label,string+labelIndex,labelLength);
    act->label[labelLength] = '\0';
    act->command = (char*)malloc(sizeof(char)*(commandLength+1));
    strncpy(act->command,string+commandIndex,commandLength);
    act->command[commandLength] = '\0';
    act->parent = parent;
    
    return act;
}

Dir* addDir(Dir* dir){
    if (allDirs==NULL) {
        allDirs = (Dir**)malloc(sizeof(Dir*)*configLines);
    }
    
    for (int d = 0; d < savedDirs; d++) {
        if (strcmp(allDirs[d]->label,dir->label) == 0) {
            if (allDirs[d]->parent == NULL) {
                allDirs[d]->parent = dir->parent;
            }
            if (allDirs[d]->key == NO_KEY) {
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
        allActs = (Act**)malloc(sizeof(Act*)*configLines);
    }
    
    return allActs[savedActs++] = act;
}

void dirStackPop() {
    dirStackTop--;
}

void dirStackPush(Dir* dir) {
    dirStackTop++;
    dirStack[dirStackTop] = dir;
}

Dir* dirStackPeek() {
    return dirStack[dirStackTop];
}

int dirStackIsEmpty() {
    return dirStackTop < 0;
}

void dirStackAlloc() {
    dirStack = (Dir**)malloc(sizeof(Dir*)*savedDirs);
}