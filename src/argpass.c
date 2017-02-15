#include<argp.h>
#include<stdlib.h>
#include<unistd.h>
#include<pwd.h>
#include<string.h>

#include"argpass.h"

//argp content
const char *argp_program_version = "Blezz 0.1";
const char *argp_program_bug_address = "<mmhmaster@hotmail.com>";
static char doc[] = "Blezz is a keybased application launcher, in a conceptual state.";

//structure for storing parameters and their descriptions
static struct argp_option options[] = {
    {"verbose", 'v',    0,           0, "Produce verbose output" },
    {"quiet",   'q',    0,           0, "Don't produce any output" },
    {"silent",  's',    0,           OPTION_ALIAS },
    {"content", 'p',    "FILE",      0, "Loads directives and actions from another file" },
    {"config",  'c',    "FILE",      0, "Loads arguments from another file" },
    {"startDir",'d',    "label",     0, "Name of the directory that should be started in"},
    { 0 }
};

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
        case 'p': //content
            arguments->contentFile = arg;
            break;
        case 'd': //startDir
            arguments->startDir = arg;
            break;
        case ARGP_KEY_ARG: //too many parameters
            if (state->arg_num >= 2) {
                argp_usage (state);
            }
            arguments->args[state->arg_num] = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

char* getHomePath() {
    char* homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return homedir;
}

//parsing information
static struct argp argp = { options, parse_opt, "", doc };

void argumentsInit() {
    char* homePath = getHomePath();
    char* defaultConfig = ".config/blezz/config";
    char* defaultContent = ".config/blezz/content";

    arguments.silent = 0;  //TODO: if 1, be silent
    arguments.verbose = 0; //TODO: if 1, be verbose
    arguments.actS = '!';
    arguments.dirS = '>';
    arguments.dirUpKey = '-';
    arguments.configFile = (char*)malloc((strlen(homePath)+strlen(defaultConfig)+1)*sizeof(char));
    sprintf(arguments.configFile,"%s/%s",homePath,defaultConfig);
    arguments.contentFile =  (char*)malloc((strlen(homePath)+strlen(defaultContent)+1)*sizeof(char));
    sprintf(arguments.contentFile,"%s/%s",homePath,defaultContent);
    arguments.font = "fixed";
    arguments.startDir = "Main";
    arguments.windowWidth = 600;
    arguments.keyAsUpper = 0;
    arguments.showMenuNames = 1;
    arguments.showMenuNamesNested = 1;
    arguments.fgColor = 0xffffff;
    arguments.bgColor = 0x000000;
    arguments.topIndent = 5;
    arguments.leftIndent = 20;
    arguments.botIndent = 10;
    arguments.winXPos = XMid;
    arguments.winYPos = YTop;
    arguments.winXOffset = 0;
    arguments.winYOffset = 0;
}

void argumentsApply(int argc, char** argv) {
    argp_parse(&argp,argc,argv,0,0,&arguments);
}