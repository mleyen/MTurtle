#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_terminal.h>

int main(void)
{
    SDL_Surface* screen;

    /* Init SDL */
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init Screen Surface */
    screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL)
    {
        fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Set Window Title */
    SDL_WM_SetCaption("Terminal Test!!", NULL);

    /* Configure Terminal */
    SDL_Terminal* term = SDL_CreateTerminal();
    SDL_TerminalSetFont(term, "trisk.ttf", 13);
    SDL_TerminalSetSize(term, 80, 25);
    SDL_TerminalSetPosition(term, 0, 0);

    SDL_TerminalSetColor(term, 18, 30, 49, 255);
    SDL_TerminalSetBorderColor(term, 18, 30, 49, 255);
    SDL_TerminalSetForeground(term, 219, 238, 255, 255);
    SDL_TerminalSetBackground(term, 0, 0, 0, 0);
    SDL_TerminalClear(term);

    SDL_TerminalPrint(term, "Hello World!\n>>> ");

    /* Main Loop */
    for(;;)
    {
        /* Check for User Events */
        SDL_Event ev;
        SDL_WaitEvent(&ev);

        /* User Exit */
        if(ev.type == SDL_QUIT)
        {
            break;
        }

        if(ev.type == SDL_TERMINALEVENT)
        {
            SDL_TerminalPrint(term, "%s\n>>> ", ev.user.data2);
        }
        else if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
        {
            break;
        }

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_TerminalBlit(term);
        SDL_Flip(screen);
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
