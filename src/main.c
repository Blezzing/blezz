#include"argpass.h"
#include"gui.h"
#include"types.h"
#include"data.h"
#include"file.h"

//gogogo!
int main(int argc, char *argv[]) {
    //Handle arguments
    setDefaultArguments();
    applyArguments(argc,argv);    

    //Parse configuration files
    Dir* root = importData(arguments.configFile);
    
    //Prepare datastructures
    dirStackAlloc();
    dirStackPush(root);
    
    //Show GUI
    guiStart();
    guiEventLoop();
    
    //Belive it or not, success!
	return 0;
}