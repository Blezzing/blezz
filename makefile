install: ./src/main.c
	gcc -Wall -g ./src/main.c -o ./bin/blezz

run: install
	./bin/blezz
