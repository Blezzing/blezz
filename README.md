# Blezz
Keystroke Launcher

# Idea
There is a multitude of nice application launchers out there, from simple and fast systems such as dmenu, to more heavy more advanced ones such as krunner. 
Where I felt these were lacking was in the way they handled integration with the desktop. If I were to make a script to split my dekstop into a preset of tiles, I would have to start the script manually or bind it to a shortcut i would forget. This launcher is a way to have shortcut like behaviour with guidance to assist your fading memory.

# Install
Go to the root dir and call make. I can not really recommend anyone using this in its current very underdeveloped phase, but you are welcome to bring any feedback if you like :)

# Configuration
Make a file with content of the form: (An example can be found in cfg/blezzrc)

\#descriptive comment
directiveTitle:
dir(char,otherDirective)
act(char,description,bashCommand)

# Run
To run the file simply call it with a path to your configuration file like this:

./bin/blezz /home/USER/blezzrc

# Controls
For now:
'ESC' -> closes the menu
'-' -> goes up a level
'a'-'z' -> bindable keys to go down a level

# Dependencies
Compiled with gcc, and using gnu c specific standard libraries (argp), so it might be a good idea to start with gcc..

# TODO
* Clean up the mess, move code to more appropiate files
* Revoke the header files, and fix the dependency mess
* Improve Makefile to include install and uninstall commands
* Resize window at every draw, to fit the lines
* Make WMs ignore window, to get free placement and no borders option
* Make controls, and config of them, sane.