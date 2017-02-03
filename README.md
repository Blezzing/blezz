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