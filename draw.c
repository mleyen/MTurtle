#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurtle.h"

struct Turtle* turt;

void click(int x, int y)
{
    TT_MoveTo(turt, x, y);
}

int main(void)
{
    TT_Init("Hello World!", 640, 480);

    turt = TT_Create(640, 480, 0, 0, 0);

    TT_PenDown(turt);
    TT_HideTurtle(turt);

    TT_OnClick(turt, &click);

    while(TT_MainLoop(turt))
    {
        /* void */
    }

    TT_Destroy(turt);

    TT_EndProgram();
    return EXIT_SUCCESS;
}
