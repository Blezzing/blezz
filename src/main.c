#include"argpass.h"
#include"gui.h"
#include"data.h"
#include"file.h"

//gogogo!
int main(int argc, char *argv[]) {
    //Prepare arguments
    argumentsInit();

    //Overwrite from arguments
    argumentsApply(argc,argv);   

    //Overwrite from config
    importConfig(arguments.configFile); 

    //Load content
    importContent(arguments.contentFile);
    
    //Prepare datastructures
    dirStackInit();
    
    //Show GUI
    guiEventLoop();
    
    //Belive it or not, success!
	return 0;
}