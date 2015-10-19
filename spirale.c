#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurtle.h"

int main(void)
{
    TT_Init("Hello World!", 640, 480);

    struct Turtle* turt = TT_Create(640, 480, 0, 0, 0);

    TT_PenDown(turt);

    float size = 1;

    while(size <= 30)
    {
        TT_Forward(turt, size);
        TT_Right(turt, 15);
        size *= 1.02;
    }

    while(TT_MainLoop(turt))
    {
        /* void */
    }

    TT_Destroy(turt);

    TT_EndProgram();
    return EXIT_SUCCESS;
}
