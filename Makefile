CPP=gcc
CFLAGS=-O3
LDFLAGS=-lSDL

all: main

main: main.o
	${CPP} $(CFLAGS) -o hello main.o ${LDFLAGS}

main.o: main.c
	${CPP} $(CFLAGS) -o main.o -c main.c


clean:	
	rm -fr *.o

mrproper: clean
	rm -fr hello
