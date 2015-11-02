#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurtle.h"

struct Turtle* turt;
int stop = 0;

void click(int x, int y)
{
    TT_HideTurtle(turt);
    TT_MoveTo(turt, x, y);
}

void keyb(SDLKey sym, SDLMod mod)
{
    TT_ShowTurtle(turt);

    switch(sym)
    {
    case SDLK_h:
        TT_Home(turt);
        break;
    case SDLK_c:
        TT_Clear(turt);
        break;
    case SDLK_r:
        TT_Reset(turt);
        break;
    case SDLK_ESCAPE:
    case SDLK_q:
        stop = 1;
        break;
    case SDLK_LEFT:
        TT_Left(turt, 9);
        break;
    case SDLK_RIGHT:
        TT_Right(turt, 9);
        break;
    case SDLK_UP:
        TT_Forward(turt, 2);
        break;
    case SDLK_DOWN:
        TT_Backward(turt, 2);
        break;
    }
}

int main(void)
{
    TT_Init("Hello World!", 640, 480);

    turt = TT_Create(640, 480, 0, 0, 0);

    TT_PenDown(turt);
    TT_HideTurtle(turt);

    TT_OnClick(turt, &click);
    TT_OnKeyb(turt, &keyb);

    while(!stop && TT_MainLoop(turt))
    {
        /* void */
    }

    TT_Destroy(turt);

    TT_EndProgram();
    return EXIT_SUCCESS;
}
