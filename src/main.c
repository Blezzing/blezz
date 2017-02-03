#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"main.h"

#define DEFAULT_CONFIG_PATH "/home/blezzing/Git/blezz/cfg/blezzrc2"
#define NO_KEY '\0'
#define LEVEL_UP_KEY '-'
#define DIR_SYMBOL '>'
#define ACT_SYMBOL '!'

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

//used to calculate maximum allocation sizes to avoid reallocs
int configLines = 0;

int savedDirs = 0;
int savedActs = 0;
Dir** allDirs = NULL;
Act** allActs = NULL;

int dirStackTop = -1;
Dir** dirStack;

void memError(){
    fprintf(stderr,"Out of memory.\n");
    exit(1);
}

void fileError(){
    fprintf(stderr,"Error opening file.\n");
    exit(2);
}

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

char** getLines(FILE* file) {
    int lines_to_allocate = 64;
    int line_size_to_allocate = 100;
    
    //check that the file is opened
    if (file==NULL) {
        fileError();
    }
    
    //allocate lines, and make sure we succeed
    char** lines = (char**)malloc(sizeof(char*)*lines_to_allocate);
    if (lines==NULL) {
        memError();
    }
    
    for (int i = 0; 1; i++) {
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
    
    configLines = sizeof(lines)/sizeof(char*);
    return lines;   
}

Dir* importStructure(char* path) {
    FILE* file;
    char** lines;
    Dir* root = NULL;
    
    printf("%s %s\n","Loading structure from:",path); 
    file = fopen(path,"r"); //TODO: add errorhandling here, missing folders give segfault
    lines = getLines(file);
    fclose(file);
    
    Dir* currDir = NULL;
    for (int i = 0; lines[i] != NULL; i++) {
        if (isDirDecl(lines[i])) {
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
    }
    
    printf("%s %i\n%s %i\n","Dirs:", savedDirs, "Acts:", savedActs);

	return root;
}

void printDir(Dir* dir) {
	if (dir == NULL) {
		printf("Empty...");
		return;
	}
	
	printf("%s:\n",dir->label);
    
    for(int i = 0; i < savedDirs; i++) {
        if (allDirs[i]->parent == dir) {
            printf("%c[%c] %s\n",DIR_SYMBOL,allDirs[i]->key, allDirs[i]->label);
        }
    }
    
    for(int i = 0; i < savedActs; i++) {
        if (allActs[i]->parent == dir) {
            printf("%c[%c] %s\n",ACT_SYMBOL,allActs[i]->key, allActs[i]->label);
        }
    }            
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

int selectElement(Dir* dir, char choice) {
    if(choice == LEVEL_UP_KEY) {
        dirStackPop();
        return dirStackIsEmpty();
    }
    
    for(int i = 0; i < savedDirs; i++) {
        if(allDirs[i]->parent == dir && allDirs[i]->key == choice){
            dirStackPush(allDirs[i]);
            return 0;
        }
    }
    
    for(int i = 0; i < savedActs; i++) {
        if (allActs[i]->parent == dir && allActs[i]->key == choice){
            system(allActs[i]->command);
            return 1;
        }
    }
    
    return 0;
}

void inputLoop() {
    int done = 0;
    while(done == 0) {
        Dir* currentDir = dirStackPeek();
        printDir(currentDir); 
        char c = fgetc(stdin);
        while(fgetc(stdin) != '\n'); //purge input
        done = selectElement(currentDir,c);
    }
}

void printDebug() {
    puts("DIRS:\n");
    for(int i=1; i< savedDirs; i++)
        printf("%c-%s>>%s\n",allDirs[i]->key,allDirs[i]->label,allDirs[i]->parent->label);
    
    
    puts("ACTS:\n");
    for(int i=0; i< savedActs; i++)
        printf("%c-%s->%s\n",allActs[i]->key,allActs[i]->label,allActs[i]->command);
}

int main(int argc, char *argv[]) {
	char* path = (argc >= 2)?argv[1]:DEFAULT_CONFIG_PATH;
    Dir* root = importStructure(path);
    
    dirStackAlloc();
    dirStackPush(root);
    
    inputLoop();
    //printDebug();
    
	return 0;
}
