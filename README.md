# Blezz - Guided Keystroke Launcher

## Idea
Complicated shortcuts are hard to remember and hard on the fingers, yet it is still nice to have shortcuts to scripts and used actions readily available.

With Blezz i intend to give an alternative to shortcut systems. It provides a little interface that shows up at launch, to guide the user through his own setup. Every keypress can go to new directory, or execute an action. It is possible for the user to make his own setup as simple or complex he wants, by adding content to a file.

As it is possible to execute shell scripts from Blezz almost any action imaginable is possible.

## Install
Compile with make and copy to bin directory with make install

```
make
sudo make install
```

## Uninstall
If copied to the bin directory, make uninstall removes it, effectively uninstalling

_Note: Any configuration file made for the program will remain_

```
sudo make uninstall
```

## Configuration
There is two possible ways to configure Blezz.
The content file is mandatory for getting any use out of the program.
The config file is providing optional information about how the program should run.

### Content file
A basic language have been formed to provide structural information about what the user wants to browse when he runs Blezz.

_Note: The first directory declared will be the starting menu, until more configuration is possible._

_Note: It does not matter if a directory is declared before or after being reffered to._

```
#This is a comment.
directoryName:                    #Defines the name of a directory for linking.
dir(char,linkedDirectory)         #Links to another directory. Binds to last mentioned directory
act(char,description,bashCommand) #Executes the command. Binds to last mentioned directory
```
(An example can be found in `cfg/content`)

### Config file
A very basic configuration file can be formed by a single type of statements.
```
name=value
```
The options for this file is: (This is being heavily expanded currently)

|Name|Value|
|---:|:----|
|font|name of an x font (find by using 'xlsfonts`|

(An example can be found in `cfg/config`)

## Run
To run the file simply call it with a path to your configuration file like this:

```
./bin/blezz -c ./cfg/config -p ./cfg/content
```

## Controls
Until better configuration of controls, this is the default.

|Key|Action|
|--:|:-----|
|ESC'|closes the menu.|
|'-'|goes up a level.|
|'A'-'Z'|select an element.|

## Dependencies
`argp` and `xcb` is the only libraries used to build this project.

## TODO
This list is not prioritized, and only shows currently known needed improvements.
* Should support keymaps. (Currently hardcoded.. yeaa..)
* Provide color options.
* Provide size options.
* Provide placement options.
* Provide better control options.
* Provide option to set another start directory.
* Click outside window should make it exit.