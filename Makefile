CPP=gcc
CFLAGS=-O3 -I /usr/local/include -I /usr/include -I /usr/include/SDL -I /usr/local/include/SDL
LDFLAGS=-lSDL -lSDL_draw -lSDL_gfx -lSDL_image -lSDL_ttf -lm

all: hello spirale draw lantern olympics

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

draw: draw.o MTurtle.o
	${CPP} $(CFLAGS) -o draw draw.o MTurtle.o ${LDFLAGS}

draw.o: draw.c
	${CPP} $(CFLAGS) -o draw.o -c draw.c

lantern: lantern.o MTurtle.o
	${CPP} $(CFLAGS) -o lantern lantern.o MTurtle.o ${LDFLAGS}

lantern.o: lantern.c
	${CPP} $(CFLAGS) -o lantern.o -c lantern.c

olympics: olympics.o MTurtle.o
	${CPP} $(CFLAGS) -o olympics olympics.o MTurtle.o ${LDFLAGS}

olympics.o: olympics.c
	${CPP} $(CFLAGS) -o olympics.o -c olympics.c

clean:	
	rm -rf *.o

mrproper: clean
	rm hello spirale draw lantern olympics
