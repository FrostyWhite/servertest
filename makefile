CC=gcc
Cflags=-I -Wall -std=c99 -pedantic -ptheread

Server: ./source/main.c ./source/render.c ./source/render.h ./source/HTTPReq.c ./HTTPReq.h
	@echo
	$(CC) -o server ./source/main.c ./source/render.c ./source/HTTPReq.c
	@echo $@ compiled
	@echo

clean:
	@echo
	rm server
	@echo executables removed
	@echo
