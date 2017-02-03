#ifndef TYPESHEADER_H
#define TYPESHEADER_H

typedef struct Dir Dir;
typedef struct Act Act;

struct Dir{
    char key;
	char* label;
    Dir* parent;
};

struct Act{
    char key;
    char* label;
	char* command;
    Dir* parent;
};

#endif