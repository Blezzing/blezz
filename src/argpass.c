#include<argp.h>
#include<stdlib.h>

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
    {"font",    'f',    "fontName",  0, "Font to use in presentation" },
    {"startDir",'d',    "label",     0, "Name of the directory that should be started in"},
    {"width",   'w',    "width",     0, "Width of the window in pixels"},
    {"actS",    -3,     "CHARACTER", 0, "Symbol to prepend actions" },
    {"dirS",    -2,     "CHARACTER", 0, "Symbol to prepend directories" },
    {"dirUpKey",-1,     "CHARACTER", 0, "Character used for going a directory up" },
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
        case 'f': //font
            arguments->font = arg;
            break;
        case 'd': //font
            arguments->startDir = arg;
            break;
        case 'w': //width
            arguments->windowWidth = (int)strtol(arg,NULL,10);
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

void argumentsInit() {
    arguments.silent = 0;  //TODO: if 1, be silent
    arguments.verbose = 0; //TODO: if 1, be verbose
    arguments.actS = '!';
    arguments.dirS = '>';
    arguments.dirUpKey = '-';
    arguments.configFile = "/home/blezzing/Git/blezz/cfg/config";
    arguments.contentFile = "/home/blezzing/Git/blezz/cfg/content";
    arguments.font = "fixed";
    arguments.startDir = "Main";
    arguments.windowWidth = 600;
    arguments.keyAsUpper = 0;
    arguments.showMenuNames = 0;
    arguments.showMenuNamesNested = 0;
}

void argumentsApply(int argc, char** argv) {
    argp_parse(&argp,argc,argv,0,0,&arguments);
}