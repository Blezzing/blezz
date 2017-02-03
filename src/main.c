#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<argp.h>

#include"types.h"
#include"data.h"
#include"main.h"
#include"errors.h"
#include"consts.h"
#include"file.h"

//argp content
const char *argp_program_version = "Blezz 0.1";
const char *argp_program_bug_address = "<mmhmaster@hotmail.com>";
static char doc[] = "Blezz is a keybased application launcher, in a conceptual state.";

//structure for storing parameters and their descriptions
static struct argp_option options[] = {
    {"verbose",  'v', 0,      0,  "Produce verbose output" },
    {"quiet",    'q', 0,      0,  "Don't produce any output" },
    {"silent",   's', 0,      OPTION_ALIAS },
    {"config",   'c', "FILE", 0,  "Loads directives and actions from another file" },
    { 0 }
};

//structure for sharing informations about arguments
struct arguments {
    char *args[2];
    int silent, verbose;
    char *configFile;
};

//parameter parsing function, following example of argp
static error_t parse_opt (int key, char *arg, struct argp_state *state){
    struct arguments *arguments = state->input;
    switch (key){
        case 'q': case 's':
            arguments->silent = 1;
            break;
        case 'v':
            arguments->verbose = 1;
            break;
        case 'c':
            arguments->configFile = arg;
            break;
        case ARGP_KEY_ARG: //too many parameters
            if (state->arg_num >= 2) {argp_usage (state);}
            arguments->args[state->arg_num] = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

//parsing information
static struct argp argp = { options, parse_opt, "", doc };

//presentation of a single directive in stdout
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

//selection in a dir, changes state if it should
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

//looping until selection completes with empty dir stack or action selection
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

//gogogo!
int main(int argc, char *argv[]) {
    struct arguments arguments;
    
    //Defaults
    arguments.silent = 0;
    arguments.verbose = 0;
    arguments.configFile = DEFAULT_CONFIG_PATH;

    //Parsing arguments
    argp_parse(&argp,argc,argv,0,0,&arguments);

    //Parsing configuration
    Dir* root = importData(arguments.configFile);
    
    //Readying data
    dirStackAlloc();
    dirStackPush(root);
    
    //Starting
    inputLoop();
    
	return 0;
}