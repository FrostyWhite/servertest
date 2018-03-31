CC=gcc
Cflags=-I -Wall -std=c99 -pedantic -ptheread

Server: ./source/main.c ./source/render.c ./source/render.h ./source/HTTPReq.c ./source/HTTPReq.h ./source/pages.c ./source/pages.h
	@echo
	$(CC) -o server ./source/main.c ./source/render.c ./source/HTTPReq.c ./source/pages.c
	@echo $@ compiled
	@echo

clean:
	@echo
	rm server
	@echo executables removed
	@echo
