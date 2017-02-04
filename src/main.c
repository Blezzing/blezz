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
    {"verbose", 'v',    0,           0, "Produce verbose output" },
    {"quiet",   'q',    0,           0, "Don't produce any output" },
    {"silent",  's',    0,           OPTION_ALIAS },
    {"config",  'c',    "FILE",      0, "Loads directives and actions from another file" },
    {"actS",    -3,     "CHARACTER", 0, "Symbol to prepend actions" },
    {"dirS",    -2,     "CHARACTER", 0, "Symbol to prepend directories" },
    {"dirUpKey",-1,     "CHARACTER", 0, "Character used for going a directory up" },
    { 0 }
};

//structure for sharing informations about arguments
struct arguments {
    char *args[2];
    int silent, verbose;
    char dirUpKey, actS, dirS;
    char *configFile;
};


static struct arguments arguments;

//parameter parsing function, following example of argp
static error_t parse_opt (int key, char *arg, struct argp_state *state){
    struct arguments *arguments = state->input;
    switch (key){
        case 'q': case 's': //silent
            arguments->silent = 1;
            break;
        case 'v': //verbose
            arguments->verbose = 1;
            break;
        case 'c': //config
            arguments->configFile = arg;
            break;
        case -1: //dirUpKey;
            arguments->dirUpKey = arg[0];
            break;
        case -2: //dirS;
            arguments->dirS = arg[0];
            break;
        case -3: //actS
            arguments->actS = arg[0];
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
            printf("%c [%c] %s\n",arguments.dirS, allDirs[i]->key, allDirs[i]->label);
        }
    }
    
    for(int i = 0; i < savedActs; i++) {
        if (allActs[i]->parent == dir) {
            printf("%c [%c] %s\n",arguments.actS, allActs[i]->key, allActs[i]->label);
        }
    }            
}

//selection in a dir, changes state if it should, if not returning 0 program should terminate.
int selectElement(Dir* dir, char choice) {
    //go dir up if dirUpKey
    if(choice == arguments.dirUpKey) {
        dirStackPop();
        return dirStackIsEmpty();
    }
    
    //go to dir if a match is found
    for(int i = 0; i < savedDirs; i++) {
        if(allDirs[i]->parent == dir && allDirs[i]->key == choice){
            dirStackPush(allDirs[i]);
            return 0;
        }
    }
    
    //perform act if a match is found
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
    //Defaults
    arguments.silent = 0;
    arguments.verbose = 0;
    arguments.actS = '!';
    arguments.dirS = '>';
    arguments.dirUpKey = '-';
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
    
    //Belive it or not, success!
	return 0;
}