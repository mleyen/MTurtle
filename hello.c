#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurt.h"

int main(void)
{
    TurtInit("Hello World!", 640, 480, 0, 0, 0);

    TurtPenDown();

    TurtForward(80);
    TurtRight(90.0f);
    TurtForward(80);
    TurtRight(90.0f);
    TurtForward(80);
    TurtRight(90.0f);
    TurtForward(80);
    TurtRight(90.0f);

    TurtWriteText("Hello World!");

    while(TurtMainLoop())
    {
        /* void */
    }

    TurtExit(EXIT_SUCCESS);
}
