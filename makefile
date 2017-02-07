CC=gcc
CFLAGS=-c -g -Wall
LDFLAGS=
SOURCES=./src/main.c ./src/errors.c ./src/data.c ./src/file.c ./src/gui.c ./src/argpass.c
LIBS=
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=./bin/blezz
INSTALLDIR=/bin/

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -lxcb -o $@

.c.o:
	$(CC) $(CFLAGS) $< -lxcb -o $@

clean:
	rm ./src/*.o

install:
	cp $(EXECUTABLE) $(INSTALLDIR)blezz

uninstall:
	rm $(INSTALLDIR)blezz