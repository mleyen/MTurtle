#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurtle.h"

int main(void)
{
    TT_Init("Turtle Olympics", 640, 480);

    struct Turtle* turt = TT_Create(640, 480, 0, 0, 0);

    TT_PenUp(turt);
    TT_MoveTo(turt, 300, 240);
    TT_PenDown(turt);
    TT_SetColor(turt, 0, 0, 255);
    TT_Circle(turt, 50);

    TT_PenUp(turt);
    TT_MoveTo(turt, 180, 240);
    TT_PenDown(turt);
    TT_SetColor(turt, 128, 0, 128);
    TT_Circle(turt, 50);

    TT_PenUp(turt);
    TT_MoveTo(turt, 360, 180);
    TT_PenDown(turt);
    TT_SetColor(turt, 255, 0, 0);
    TT_Circle(turt, 50);

    TT_PenUp(turt);
    TT_MoveTo(turt, 240, 180);
    TT_PenDown(turt);
    TT_SetColor(turt, 255, 255, 0);
    TT_Circle(turt, 50);

    TT_PenUp(turt);
    TT_MoveTo(turt, 120, 180);
    TT_PenDown(turt);
    TT_SetColor(turt, 0, 255, 0);
    TT_Circle(turt, 50);

    while(TT_MainLoop(turt))
    {
        /* void */
    }

    TT_Destroy(turt);

    TT_EndProgram();
    return EXIT_SUCCESS;
}
