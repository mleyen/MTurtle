/*
 * Copyright 2015 Mathias Leyendecker / University of Strasbourg
 *
 * This file is part of MTurtle.
 * MTurtle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MTurtle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MTurtle.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ====================================================================
 *
 * MTurtle Console
 * Command line interpreter for Turtle graphics
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_terminal.h>
#include "MTurtle.h"
#include "MTurtleConsole.h"

#define TERMINAL_FONT_FILE "miscfixed.ttf"
#define TERMINAL_FONT_SIZE 12

SDL_Surface* screen;
struct Turtle* turt;
SDL_Terminal* term;

int main(int argc, char** argv)
{
    /* Init SDL */
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init Screen Surface */
    screen = SDL_SetVideoMode(960, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL)
    {
        fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init SDL_ttf */
    if(TTF_Init() == -1)
    {
        fprintf(stderr, "TTF_Init() failed: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    /* Set Window Title */
    SDL_WM_SetCaption("Turtle Graphics Console", NULL);

    /* Init MTurtle */
    TT_InitMinimal(screen);
    turt = TT_Create(640, 480, 0, 0, 0);
    TT_SetSurfacePos(turt, 320, 0);

    /* Init Terminal */
    term = SDL_CreateTerminal();
    SDL_TerminalSetFont(term, TERMINAL_FONT_FILE, TERMINAL_FONT_SIZE);
    SDL_TerminalSetSize(term, 80, 25);
    SDL_TerminalSetPosition(term, 10, 10);
    SDL_TerminalClear(term);

    SDL_TerminalPrint(term, "MTurtle Console\n\xA9 2015 Mathias Leyendecker\n>>> ");

    /* Make Test Drawing */
    TT_PenDown(turt);

    float size = 1;

    while(size <= 30)
    {
        TT_Forward(turt, size);
        TT_Right(turt, 15);
        size *= 1.02;
    }

    TT_MoveTo(turt, 0, 0);

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

        /* Process Terminal Events */
        if(ev.type == SDL_TERMINALEVENT)
        {
            if(strcmp(ev.user.data2, "exit") == 0)
            {
                break;
            }
            else
            {
                SDL_TerminalPrint(term, "%s\n>>> ", ev.user.data2);
            }
            /* TODO */
        }
        /*else if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
        {
            break;
        }*/

        /* Clear Screen */
        SDL_FillRect(screen, NULL, 0);

        /* Blit Turtle */
        TT_Blit(turt);

        /* Blit Terminal */
        SDL_TerminalBlit(term);

        /* Refresh Screen */
        SDL_Flip(screen);
    }

    TT_Destroy(turt);
    SDL_DestroyTerminal(term);
    TT_EndProgram();
    return EXIT_SUCCESS;
}
