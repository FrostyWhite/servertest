CC=gcc
Cflags=-I -Wall -std=c99 -pedantic -ptheread

all: Daemon Tester

Server: ./main.c
	@echo
	$(CC) -o server ./main.c
	@echo $@ compiled
	@echo

clean:
	@echo
	rm server
	@echo executables removed
	@echo
