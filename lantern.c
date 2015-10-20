#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurtle.h"

int main(void)
{
    TT_Init("Hello World!", 640, 480);

    struct Turtle* turt = TT_Create(640, 480, 0, 0, 0);

    TT_PenDown(turt);

    int l = 100;

    TT_Left(turt, 90);
    TT_Forward(turt, l);
    TT_Right(turt, 90);
    TT_Forward(turt, l);
    TT_Right(turt, 90);
    TT_Forward(turt, l);
    TT_Right(turt, 90);
    TT_Forward(turt, l);
    TT_Right(turt, 135);
    TT_Forward(turt, l * sqrt(2));
    TT_Left(turt, 90);

    TT_BeginFill(turt, 3, 255, 0, 0);
    TT_Forward(turt, l * sqrt(2) / 2);
    TT_Left(turt, 90);
    TT_Forward(turt, l * sqrt(2) / 2);
    /* End Fill */

    TT_Left(turt, 90);
    TT_Forward(turt, l * sqrt(2));
    TT_Right(turt, 45);

    while(TT_MainLoop(turt))
    {
        /* void */
    }

    TT_Destroy(turt);

    TT_EndProgram();
    return EXIT_SUCCESS;
}
