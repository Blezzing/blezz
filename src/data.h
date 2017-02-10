#ifndef DATAHEADER_H
#define DATAHEADER_H

//Defines for return variable checking
#define ELEMENT_SELECTION_FALSE 0
#define ELEMENT_SELECTION_TRUE 1
#define ELEMENT_SELECTION_OVER 2


//Structures
typedef struct Dir Dir;
typedef struct Act Act;

struct Dir{
    char key;
	char* label;
    Dir* parent;
};

struct Act{
    char key;
    char* label;
	char* command;
    Dir* parent;
};

//Shared variables
int   savedDirs;
int   savedActs;
Dir** allDirs;
Act** allActs;

int   dirStackTop;
Dir** dirStack;

Dir* startDir;

//Functions
int isDirDecl(char* string);
int isDirRef(char* string);
int isActRef(char* string);

Dir* newDirFromRef(char* string, Dir* parent);
Dir* newDirFromDecl(char* string);
Act* newActFromRef(char* string, Dir* parent);

Dir* addDir(Dir* dir);
Act* addAct(Act* act);

void dirStackInit();
void dirStackPush(Dir* dir);
void dirStackPop();
Dir* dirStackPeek();
int  dirStackIsEmpty();

char** allocForDirToStrings();
char** dirToStrings(char** ret, int* count);
int    selectElement(char choice);

#endif