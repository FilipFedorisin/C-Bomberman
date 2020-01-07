# variables
CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror
LDLIBS=-lm -lcurses
OUTPUT=game

# targets
all: $(OUTPUT)

$(OUTPUT): bomber.o main.o
	cppcheck --enable=performance,unusedFunction --error-exitcode=1 *.c
	$(CC) $(CFLAGS) bomber.o main.o $(LDLIBS) -o $(OUTPUT)
	rm -rf bomber.o
	rm -rf main.o


main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(LDLIBS) -o main.o

bomber.o: bomber.c bomber.h structs.h
	$(CC) $(CFLAGS) -c bomber.c $(LDLIBS) -o bomber.o


