#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DEFAULT_CONFIG_PATH "/home/blezzing/Git/blezz/cfg/blezzrc"

typedef struct Menu Menu;
typedef struct Action Action;
typedef struct Item Item;

struct Menu{
	char* title;
    Item* link;
};

struct Action{
	char* command;
    Item* link;
};

struct Item{
	char key;
	char* label;
    Menu* menu;
};

int allMenusAllocationSize = 10;
int allMenusCount = 0;
Menu** allMenus = NULL;

int allActionsAllocationSize = 10;
int allActionsCount = 0;
Action** allActions = NULL;

int allItemsAllocationSize = 10;
int allItemsCount = 0;
Item** allItems = NULL;


void memError(){
    fprintf(stderr,"Out of memory.\n");
    exit(1);
}

void fileError(){
    fprintf(stderr,"Error opening file.\n");
    exit(2);
}

void addMenu(Menu* menu) {
    if (allMenus==NULL) {
        allMenus=(Menu**)malloc(sizeof(Menu*)*allMenusAllocationSize);
        if (allMenus==NULL) {
            memError();
        }
    }
    
    if (allMenusCount + 1 >= allMenusAllocationSize) {
        allMenusAllocationSize = allMenusAllocationSize * 2;
        allMenus=(Menu**)realloc(allMenus,sizeof(Menu*)*allMenusAllocationSize);
    }
    
    allMenus[allMenusCount++] = menu;
}

void addAction(Action* action) {
    if (allActions==NULL) {
        allActions=(Action**)malloc(sizeof(Action*)*allActionsAllocationSize);
        if (allActions==NULL) {
            memError();
        }
    }
    
    if (allActionsCount + 1 >= allActionsAllocationSize) {
        allActionsAllocationSize = allActionsAllocationSize * 2;
        allActions=(Action**)realloc(allActions,sizeof(Action*)*allActionsAllocationSize);
    }
    
    allActions[allActionsCount++] = action;
}

void addItem(Item* item) {
    if (allItems==NULL) {
        allItems=(Item**)malloc(sizeof(Item*)*allItemsAllocationSize);
        if (allItems==NULL) {
            memError();
        }
    }
    
    if (allItemsCount + 1 >= allItemsAllocationSize) {
        allItemsAllocationSize = allItemsAllocationSize * 2;
        allItems=(Item**)realloc(allItems,sizeof(Item*)*allItemsAllocationSize);
    }
    
    allItems[allItemsCount++] = item;
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
        if(lines==NULL) {
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
    
    return lines;   
}

int isMenu(char* string) {
    return (strlen(string) > 5 && strncmp(string,"menu:",5) == 0);
}

int isAction(char* string) {
    return (strlen(string) > 8 && strncmp(string,"action:",7) == 0);
}

int isItem(char* string) {
    return (strlen(string) > 8 && string[1] == ':' && string[strlen(string)-1] == ')' && strcspn(string,"(") != strlen(string));
}

Menu* newMenu(char* string, Item* item) {
    if (isMenu(string)) {
        char* mTitle = (char*)malloc(sizeof(char)*(strlen(string)-5));
        strncpy(mTitle,string+5,strlen(string)-4);
        
        for (int i = 0; i < allMenusCount; i++)
        {
            if (strcmp(allMenus[i]->title,mTitle)==0) {
                if(allMenus[i]->link==NULL) {
                    allMenus[i]->link = item;
                }
                
                free(mTitle);
                return allMenus[i];
            }
        }
        
        Menu* menu = (Menu*)malloc(sizeof(Menu));
        menu->title = mTitle;
        menu->link = item;
        
        addMenu(menu);
        
        return menu;
    }

    return NULL;
}

Action* newAction(char* string, Item* item) {
    if (isAction(string)) {
        char* aCommand = (char*)malloc(sizeof(char)*(strlen(string)-6));
        strncpy(aCommand,string+7,strlen(string)-6);
        aCommand[strlen(string)-7] = '\0';
        
        Action* action = (Action*)malloc(sizeof(Action));
        action->command = aCommand;
        action->link = item;
        
        addAction(action);
        
        return action;
    }
    return NULL;
}

Item* newItem(char* string, Menu* iMenu) {
    if (isItem(string) && !(iMenu == NULL)) {
        char iKey = string[0];
        int parIndex = strcspn(string,"(");
        char* iLabel = (char*)malloc(sizeof(char)*(parIndex-3));
        strncpy(iLabel,string+2,parIndex-2);
        iLabel[parIndex-2]='\0';
        
        char* content = (char*)malloc(sizeof(char)*(strlen(string) - (parIndex)));
        strncpy(content,string+parIndex+1,(strlen(string) - (parIndex + 2)));
        content[(strlen(string) - (parIndex + 2))]='\0';
        
        Item* item = (Item*)malloc(sizeof(Item));
        item->key = iKey;
        item->menu = iMenu;
        item->label = iLabel;
        
        addItem(item);
        
        if (isAction(content)) {
            newAction(content,item);
        }
        
        if (isMenu(content)) {
            newMenu(content,item);
        }
        
        return item;
    }
    
    return NULL;
}

Menu* buildMenuFromFile(char* path) {
    FILE* file;
    char** lines;
    Menu* rootMenu = NULL;
    
    printf("%s %s","Loading structure from:",path); 
    file = fopen(path,"r"); //TODO: add some sort of errorhandling here, missing folders give segfault
    lines = getLines(file);
    fclose(file);

    //type checking happens in constructors
    Menu* currMenu = NULL;
    for (int i = 0; lines[i] != NULL; i++) {
        Menu* menu = newMenu(lines[i],NULL);
        if (!(menu==NULL)) {
            if(currMenu==NULL) {
                rootMenu=menu;
            }
            
            currMenu=menu;
            continue;
        }
        
        newItem(lines[i],currMenu);
    }
    
    printf("%s %i\n%s %i\n%s %i\n","Items:", allItemsCount, "Menus:", allMenusCount, "Actions:", allActionsCount);

	return rootMenu;
}

void printMenu(Menu* thisMenu) {
	if (thisMenu == NULL) {
		printf("Empty menu...");
		return;
	}
	
	printf("%s:\n",thisMenu->title);
    for(int i = 0; i < allItemsCount; i++) {
        if ((allItems[i]->menu) == thisMenu) {
            printf("[%c] %s\n",allItems[i]->key, allItems[i]->label);
        }
    }
}

Menu* currentMenu;
int selectElement(Menu* inMenu, char choice) {
    for(int i = 0; i < allItemsCount; i++) {
        if(allItems[i]->menu == inMenu && allItems[i]->key == choice){
            for(int m = 0; m < allMenusCount; m++) {
                if (allMenus[m]->link == allItems[i]) {
                    currentMenu = allMenus[m];
                }
            }
            for(int a = 0; a < allActionsCount; a++) {
                if (allActions[a]->link == allItems[i]) {
                    system(allActions[a]->command);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
	char* path = (argc >= 2)?argv[1]:DEFAULT_CONFIG_PATH;
    currentMenu = buildMenuFromFile(path);
    
    int done = 0;
    while(done == 0) {
        printMenu(currentMenu); 
        char c = fgetc(stdin);
        while(fgetc(stdin) != '\n'); //purge input
        done = selectElement(currentMenu,c);
    }
    
	return 0;
}
