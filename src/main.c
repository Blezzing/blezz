#include"argpass.h"
#include"gui.h"
#include"data.h"
#include"file.h"
#include"mutex.h"

//gogogo!
int main(int argc, char *argv[]) {
    //try to get lock
    applicationLock();
    if(applicationFirstInstance() == 0) {
        return 0;
    }

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