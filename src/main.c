#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"types.h"
#include"data.h"
#include"main.h"
#include"errors.h"
#include"consts.h"
#include"file.h"

void printDir(Dir* dir) {
	if (dir == NULL) {
		printf("Empty...");
		return;
	}
	
	printf("%s:\n",dir->label);
    
    for(int i = 0; i < savedDirs; i++) {
        if (allDirs[i]->parent == dir) {
            printf("%c [%c] %s\n",DIR_SYMBOL, allDirs[i]->key, allDirs[i]->label);
        }
    }
    
    for(int i = 0; i < savedActs; i++) {
        if (allActs[i]->parent == dir) {
            printf("%c [%c] %s\n",ACT_SYMBOL, allActs[i]->key, allActs[i]->label);
        }
    }            
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

int main(int argc, char *argv[]) {
	char* path = (argc >= 2)?argv[1]:DEFAULT_CONFIG_PATH;
    Dir* root = importData(path);
    
    dirStackAlloc();
    dirStackPush(root);
    
    inputLoop();
    
	return 0;
}