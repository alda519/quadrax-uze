PROJECT=quadrax

CC=gcc

CFLAGS=-std=c99 -Wall -Wextra
LIBS=-lSDL

.PHONY: build clean dep

build: dep.list $(PROJECT)

$(PROJECT): quadrax.o game.o graphics.o boulder.o
	$(CC) $(CFLAGS) $(LIBS) -o $@  $^

dep.list:
	$(CC) -MM *.c > dep.list

-include dep.list

clean:
	-rm *.o dep.list $(PROJECT)
