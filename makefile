CC=gcc
CFLAGS=-c -g -Wall
LDFLAGS=
SOURCES=./src/main.c ./src/errors.c ./src/data.c ./src/file.c ./src/gui.c ./src/argpass.c ./src/keys.c
LIBS=-lxcb
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=./blezz
INSTALLDIR=/usr/bin/

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS)  -o $@

.c.o:
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f ./src/*.o

run:
	$(EXECUTABLE)

install:
	cp $(EXECUTABLE) $(INSTALLDIR)blezz

uninstall:
	rm $(INSTALLDIR)blezz

re: clean all