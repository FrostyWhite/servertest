CC=gcc
Cflags=-I -Wall -std=c99 -pedantic -ptheread

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
