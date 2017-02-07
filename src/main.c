#include"argpass.h"
#include"gui.h"
#include"data.h"
#include"file.h"

//gogogo!
int main(int argc, char *argv[]) {
    //Handle arguments
    argumentsInit();
    argumentsApply(argc,argv);    

    //Parse configuration files
    importData(arguments.configFile);
    
    //Prepare datastructures
    dirStackInit();
    
    //Show GUI
    guiStart();
    guiEventLoop();
    
    //Belive it or not, success!
	return 0;
}