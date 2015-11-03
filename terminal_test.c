#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
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
    screen = SDL_SetVideoMode(800, 533, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL)
    {
        fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Set Window Title */
    SDL_WM_SetCaption("Terminal Test!!", NULL);

    /* Configure Terminal */
    SDL_Terminal* term = SDL_CreateTerminal();
    if(SDL_TerminalSetFont(term, "miscfixed.ttf", 12) == -1)
    {
        fprintf(stderr, "SDL_TerminalSetFont() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_TerminalSetSize(term, 80, 25);
    SDL_TerminalSetPosition(term, 0, 0);

    /*SDL_TerminalSetColor(term, 18, 30, 49, 255);
    SDL_TerminalSetBorderColor(term, 18, 30, 49, 255);
    SDL_TerminalSetForeground(term, 219, 238, 255, 255);
    SDL_TerminalSetBackground(term, 0, 0, 0, 0);
    SDL_TerminalClear(term);*/

    SDL_TerminalClear (term);
    SDL_TerminalPrint (term, "Terminal initialized\n");
    SDL_TerminalPrint (term, "Using font %s, %d\n", term->font_filename, term->font_size);
    SDL_TerminalPrint (term, "Terminal geometry: %d x %d\n\n", term->size.column, term->size.row);
    SDL_TerminalPrint (term, "\033[1mBold on\033[22m - Bold off\n");
    SDL_TerminalPrint (term, "\033[3mItalic on\033[23m - Italic off\n");
    SDL_TerminalPrint (term, "\033[4mUnderline on\033[24m - Underline off\n");

    SDL_TerminalPrint (term,
                       "\033[30mBlack"
                       "\033[31m - Red"
                       "\033[32m - Green"
                       "\033[33m - Yellow"
                       "\033[34m - Blue"
                       "\033[35m - Magenta"
                       "\033[36m - Cyan"
                       "\033[37m - White\033[30m\n");
    SDL_TerminalReset(term);
    SDL_TerminalPrint (term,
                       "\033[40;37mBlack\033[30m"
                       "\033[49m - \033[41mRed"
                       "\033[49m - \033[42mGreen"
                       "\033[49m - \033[43mYellow"
                       "\033[49m - \033[44mBlue"
                       "\033[49m - \033[45mMagenta"
                       "\033[49m - \033[46mCyan"
                       "\033[49m - \033[47mWhite\033[49m\n");


    SDL_TerminalPrint(term, "Hello World!\n>>> ");

    /* Configure Background */
    SDL_Surface* bg = IMG_Load("rsz_edubuntu-northern-lights-full.jpg");
    SDL_Rect bgpos;
    bgpos.x = 0;
    bgpos.y = 0;

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

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(bg, NULL, screen, &bgpos);
        SDL_TerminalBlit(term);
        SDL_Flip(screen);
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
