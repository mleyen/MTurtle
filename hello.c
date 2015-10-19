#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurtle.h"

int main(void)
{
    TT_Init("Hello World!", 640, 480);

    struct Turtle* turt = TT_Create(640, 480, 0, 0, 0);

    TT_PenDown(turt);

    TT_Forward(turt, 80);
    TT_Right(turt, 90.0f);
    TT_Forward(turt, 80);
    TT_Right(turt, 90.0f);
    TT_Forward(turt, 80);
    TT_Right(turt, 90.0f);
    TT_Forward(turt, 80);
    TT_Right(turt, 90.0f);

    while(TT_MainLoop(turt))
    {
        /* void */
    }

    TT_Destroy(turt);

    TT_EndProgram();
    return EXIT_SUCCESS;
}
