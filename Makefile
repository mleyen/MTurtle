CPP=gcc
CFLAGS=-O3 -I /usr/local/include -I /usr/include -I /usr/include/SDL -I /usr/local/include/SDL
LDFLAGS=-lSDL -lSDL_draw -lSDL_gfx -lSDL_image -lSDL_ttf -lm

all: hello spirale

hello: hello.o
	${CPP} $(CFLAGS) -o hello hello.o ${LDFLAGS}

hello.o: hello.c
	${CPP} $(CFLAGS) -o hello.o -c hello.c

spirale: spirale.o
	${CPP} $(CFLAGS) -o spirale spirale.o ${LDFLAGS}

spirale.o: spirale.c
	${CPP} $(CFLAGS) -o spirale.o -c spirale.c

clean:	
	rm -rf *.o

mrproper: clean
	rm -rf hello spirale
