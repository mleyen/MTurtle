#ifndef __MTURTLE_H_
#define __MTURTLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL/SDL.h>
#include "SDL_draw.h"

SDL_Surface* tt_screen;

/**
 * The Turtle struct. Describes a turtle cursor, with its position,
 * orientation, color, and associated SDL surface.
 */
struct Turtle
{
    int x;                      /* cursor position (X) */
    int y;                      /* cursor position (Y) */
    float angle;                /* cursor orientation */
    bool isDrawing;             /* is the pen up or down? */
    bool isVisible;             /* is the turtle visible or not? */
    Uint32 color;               /* line color */
    Uint32 cursorColor;         /* turtle color */
    Uint32 bgColor;             /* fill color */
    SDL_Surface* surface;       /* SDL surface for the turtle screen */
    SDL_Surface* cursorSurface; /* SDL surface for the turtle cursor */
};

/**
 * Initializes the MTurtle library. Must be called before any other
 * function.
 * @param title window title
 * @param w window width
 * @param h window height
 */
void TT_Init(const char* title, int w, int h)
{
    /* Init SDL */
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "SDL_Init() failed\: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init Screen Surface */
    tt_screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);
    if(tt_screen == NULL)
    {
        fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Set Window Title */
    SDL_WM_SetCaption(title, NULL);

    /* TODO Make Default Cursor Surface */
}

/**
 * Creates a new turtle.
 * @param w surface width
 * @param h surface height
 * @param r background color - red component
 * @param g background color - green component
 * @param b background color - blue component
 * @return New Turtle struct
 */
struct Turtle* TT_Create(int w, int h, int r, int g, int b)
{
    struct Turtle* turt = malloc(sizeof(struct Turtle));

    /* Default Parameters */
    turt->x = w / 2;
    turt->y = h / 2;
    turt->angle = 0;
    turt->isDrawing = false;
    turt->isVisible = true;
    turt->color = SDL_MapRGB(255, 255, 255); /* white */
    turt->cursorColor = SDL_MapRGB(255, 0, 0); /* red */
    turt->bgColor = SDL_MapRGB(r, g, b);
    turt->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32, 0, 0, 0, 0);
    /*turt->cursorSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32, 0, 0, 0, 0);*/
    /* TODO find sprite for cursor */

    /* Error Control */
    if(turt->surface == NULL /*|| turt->cursorSurface == NULL*/)
    {
        fprintf(stderr, "TT_Create() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init Surface */
    SDL_FillRect(turt->surface, NULL, turt->bgColor);

    return turt;
}

/**
 * Waits for the user to close the window or press ESC / Q.
 */
void TT_WaitUserExit()
{
    int stop = 0;
    SDL_Event ev;

    while(!stop)
    {
        SDL_WaitEvent(&ev);
        if(ev.type == SDL_QUIT)
        {
            stop = 1;
        }
    }
}

/**
 * Check if we should keep running the application
 * @return true until the user closes the window or presses ESC / Q
 */
bool TT_MainLoop()
{
    SDL_Event ev;
    SDL_PollEvent(&ev);

    if(ev.type == SDL_QUIT)
    {
        return false;
    }

    if(ev.type == SDL_KEYDOWN)
    {
        SDLKey sym = ev.key.keysym.sym;
        if(sym == SDLK_ESCAPE || sym == SDLK_q)
        {
            return false;
        }
    }

    return true;
}

/**
 * Destroys a Turtle struct.
 * @param turt the victim
 */
void TT_Destroy(struct Turtle* turt)
{
    SDL_FreeSurface(turt->surface);
    free(turt);
}

/**
 * Uninitializes the SDL. Call this at the end of your program, before
 * `return EXIT_SUCCESS'.
 */
void TT_EndProgram()
{
    SDL_Quit();
}

/**
 * Sets the line color for the given turtle
 * @param turt
 * @param r
 * @param g
 * @param b
 */
void TT_SetColor(struct Turtle* turt, Uint32 r, Uint32 g, Uint32 b)
{
    turt->color = SDL_MapRGB(turt->surface->format, r, g, b);
}

/**
 * Moves the cursor to the specified location
 * @param turt
 * @param x
 * @param y
 */
void TT_MoveTo(struct Turtle* turt, int x, int y)
{
    /* Draw Line as Necessary */
    if(turt->isDrawing)
    {
        Draw_Line(turt->surface, turt->x, turt->y, x, y, turt->color);
        /* TODO rotate cursor */
    }

    turt->x = x;
    turt->y = y;
}

/**
 * Rotates the turtle left
 * @param turt
 * @param deg
 */
void TT_Left(struct Turtle* turt, float deg)
{
    turt->angle = (turt->angle + deg) % 360f; /* counterclockwise */
}

/**
 * Rotates the turtle right
 * @param turt
 * @param deg
 */
void TT_Right(struct Turtle* turt, float deg)
{
    turt->angle = (turt->angle - deg) % 360f; /* clockwise */
}

/**
 * Moves the turtle forward
 * @param turt
 * @param distance
 */
void TT_Forward(struct Turtle* turt, int distance)
{
    /* Evaluate Dest Location from Polar Coordinates */
    int x = turt->x + (distance * cos(turt->angle));
    int y = turt->y + (distance * sin(turt->angle));

    TT_MoveTo(turt, x, y);
}

void TT_Backward(struct Turtle* turt, int distance)
{
    TT_Forward(turt, -distance);
}

void TT_Clear(struct Turtle* turt)
{
    /* Init Surface */
    SDL_FillRect(turt->surface, NULL, turt->bgColor);
}

void TT_PenUp(struct Turtle* turt)
{
    turt->isDrawing = false;
}

void TT_PenDown(struct Turtle* turt)
{
    turt->isDrawing = true;
}

#endif /* __MTURTLE_H_ */
