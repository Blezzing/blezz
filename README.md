# Blezz
Keystroke Launcher

# Idea
There is a multitude of nice application launchers out there, from simple and fast systems such as dmenu, to more heavy more advanced ones such as krunner. 
Where I felt these were lacking was in the way they handled integration with the desktop. If I were to make a script to split my dekstop into a preset of tiles, I would have to start the script manually or bind it to a shortcut i would forget. This launcher is a way to have shortcut like behaviour with guidance to assist your fading memory.

# Install
make
sudo make install

# Uninstall
sudo make uninstall

# Configuration
Make a file with content of the form: (An example can be found in cfg/content)

\#descriptive comment
directiveTitle:
dir(char,otherDirective)
act(char,description,bashCommand)

# Run
To run the file simply call it with a path to your configuration file like this:

./bin/blezz -c /home/USER/content

# Controls
For now:
'ESC' -> closes the menu.
'-' -> goes up a level.
'a'-'z' -> bindable keys to go down a level.

# Dependencies
Compilation: argp and xcb headers.
Run: xcb.

# TODO
* Clean up the mess, move code to more appropiate files.
* Revoke the header files, and fix the dependency mess.
* Make better support for multiple keymaps.
* Make controls, and config of them, sane.