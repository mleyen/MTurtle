CPP=gcc
CFLAGS=-O3 -I /usr/local/include -I /usr/include -I /usr/include/SDL -I /usr/local/include/SDL
LDFLAGS=-lSDL -lSDL_draw -lSDL_gfx -lSDL_image -lSDL_ttf -lm

all: hello spirale

#MTurtle: MTurtle.o
#	${CPP} $(CFLAGS) -o hello hello.o ${LDFLAGS}

MTurtle.o: MTurtle.c
	${CPP} $(CFLAGS) -o MTurtle.o -c MTurtle.c

hello: hello.o MTurtle.o
	${CPP} $(CFLAGS) -o hello hello.o MTurtle.o ${LDFLAGS}

hello.o: hello.c
	${CPP} $(CFLAGS) -o hello.o -c hello.c

spirale: spirale.o MTurtle.o
	${CPP} $(CFLAGS) -o spirale spirale.o MTurtle.o ${LDFLAGS}

spirale.o: spirale.c
	${CPP} $(CFLAGS) -o spirale.o -c spirale.c

clean:	
	rm -rf *.o

mrproper: clean
	rm -rf hello spirale
