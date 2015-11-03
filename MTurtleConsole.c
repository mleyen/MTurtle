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
    if(SDL_TerminalSetFont(term, TERMINAL_FONT_FILE, TERMINAL_FONT_SIZE) == -1)
    {
        fprintf(stderr, "SDL_TerminalSetFont() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_TerminalSetSize(term, 80, 25);
    SDL_TerminalSetPosition(term, 10, 10);
    SDL_TerminalClear(term);

    SDL_TerminalPrint(term, "MTurtle Console\n\xA9 2015 Mathias Leyendecker\n>>> ");

    char buf[1024];

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
            strncpy(buf, ev.user.data2, 1024);
            if(!parseLine(buf))
            {
                break;
            }
            SDL_TerminalPrint(term, ">>> ");
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

bool parseLine(char* str)
{
    char* token;
    char* oldstr = str;
    token = strtok(str, " \t");

    /* Empty Command */
    if(token == NULL)
    {
        /* Do Nothing */
    }
    /* Exit */
    else if(strcmp(token, "exit") == 0 || strcmp(token, "quitte") == 0)
    {
        return false;
    }
    /* Help */
    else if(strcmp(token, "help") == 0 || strcmp(token, "aide") == 0)
    {
        SDL_TerminalPrint(term, "Write me!\n");
    }
    /* Forward */
    else if(strcmp(token, "forward") == 0 || strcmp(token, "fwd") == 0 || strcmp(token, "avance") == 0)
    {
        int distance = 0;
        token = strtok(NULL, " \t");
        if(token == NULL)
        {
            distance = 20;
        }
        else
        {
            distance = strtol(token, NULL, 0);
            if(distance == 0)
            {
                distance = 20;
            }
        }

        TT_Forward(turt, distance);
    }
    /* Backward */
    else if(strcmp(token, "backward") == 0 || strcmp(token, "back") == 0 || strcmp(token, "recule") == 0)
    {
        int distance = 0;
        token = strtok(NULL, " \t");
        if(token == NULL)
        {
            distance = 20;
        }
        else
        {
            distance = strtol(token, NULL, 0);
            if(distance == 0)
            {
                distance = 20;
            }
        }

        TT_Backward(turt, distance);
    }
    /* Left */
    else if(strcmp(token, "left") == 0 || strcmp(token, "gauche") == 0)
    {
        int angle = 0;
        token = strtok(NULL, " \t");
        if(token == NULL)
        {
            angle = 90;
        }
        else
        {
            angle = strtol(token, NULL, 0);
            if(angle == 0)
            {
                angle = 20;
            }
        }

        TT_Left(turt, angle);
    }
    /* Right */
    else if(strcmp(token, "right") == 0 || strcmp(token, "droite") == 0)
    {
        int angle = 0;
        token = strtok(NULL, " \t");
        if(token == NULL)
        {
            angle = 90;
        }
        else
        {
            angle = strtol(token, NULL, 0);
            if(angle == 0)
            {
                angle = 90;
            }
        }

        TT_Right(turt, angle);
    }
    /* Pen Down */
    else if(strcmp(token, "down") == 0 || strcmp(token, "pendown") == 0 || strcmp(token, "baissecrayon") == 0)
    {
        TT_PenDown(turt);
    }
    /* Pen Up */
    else if(strcmp(token, "up") == 0 || strcmp(token, "penup") == 0 || strcmp(token, "levecrayon") == 0)
    {
        TT_PenUp(turt);
    }
    /* Show Turtle */
    else if(strcmp(token, "showturtle") == 0 || strcmp(token, "montretortue") == 0)
    {
        TT_ShowTurtle(turt);
    }
    /* Hide Turtle */
    else if(strcmp(token, "hideturtle") == 0 || strcmp(token, "cachetortue") == 0)
    {
        TT_HideTurtle(turt);
    }
    /* Circle */
    /*
     * FIXME will cause segfaults and display corruption on big values
     * (e.g. circle of radius 237 from the origin)
     */
    else if(strcmp(token, "circle") == 0 || strcmp(token, "circ") == 0 || strcmp(token, "cercle") == 0)
    {
        int radius = 0;
        token = strtok(NULL, " \t");
        if(token == NULL)
        {
            radius = 20;
        }
        else
        {
            radius = strtol(token, NULL, 0);
            if(radius == 0)
            {
                radius = 20;
            }
        }

        TT_Circle(turt, radius);
    }
    /* Write Text */
    else if(strcmp(token, "write") == 0 || strcmp(token, "ecrire") == 0 || strcmp(token, "ecrit") == 0)
    {
        token = strtok(NULL, "");
        TT_WriteText(turt, token);
    }
    /* Home */
    else if(strcmp(token, "home") == 0 || strcmp(token, "origine") == 0)
    {
        TT_Home(turt);
    }
    /* Clear */
    else if(strcmp(token, "clear") == 0 || strcmp(token, "efface") == 0)
    {
        TT_Clear(turt);
    }
    /* Reset */
    else if(strcmp(token, "reset") == 0)
    {
        TT_Reset(turt);
    }
    /* Echo */
    else if(strcmp(token, "echo") == 0)
    {
        token = strtok(NULL, "");
        SDL_TerminalPrint(term, "%s\n", token);
    }
    /* Xyzzy */
    else if(strcmp(token, "xyzzy") == 0)
    {
        /* Do Nothing */
    }
    /* Unknown Command */
    else
    {
        SDL_TerminalPrint(term, "%s: no such command.\n", token);
    }

    str = oldstr;
    return true;
}
