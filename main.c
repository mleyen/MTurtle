#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include "MTurtle.h"

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("Hello World!", NULL);

    TT_WaitUserExit();

    SDL_Quit();

    return EXIT_SUCCESS;
}

