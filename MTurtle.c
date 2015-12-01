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
 * MTurtle
 * Turtle graphics library for C using SDL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "SDL_rotozoom.h"
#include "SDL_gfxPrimitives.h"
#include "SDL_draw.h"
#include "MTurtle.h"

#define BITS_PER_PIXEL 32
#define PI 3.14159265
#define RAD2DEG PI / 180.0

/* TODO make these configurable at runtime? */
#define TURTLE_SPRITE "triangle3.png"
#define FONT_FILE "miscfixed.ttf"
#define FONT_SIZE 12 /* in pt */

SDL_Surface* tt_screen;
SDL_Surface* tt_baseCursor;
TTF_Font* tt_font;


/**
 * Change from an "int color" to an SDL_Color
 * via http://www.gamedev.net/topic/291015-cannot-convert-from-uint32-to-sdl_color/
 * @param int_color
 * @return
 */
SDL_Color translate_color(Uint32 int_color)
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        SDL_Color color={(int_color & 0x00ff0000)/0x10000,(int_color &
0x0000ff00)/0x100,(int_color & 0x000000ff),0};
    #else
        SDL_Color color={(int_color & 0x000000ff),(int_color &
0x0000ff00)/0x100,(int_color & 0x00ff0000)/0x10000,0};
    #endif
    return color;
}

float mod(float x, float y)
{
    float r = fmod(x, y);
    return r < 0 ? r + y : r;
}


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
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init Screen Surface */
    tt_screen = SDL_SetVideoMode(w, h, BITS_PER_PIXEL, SDL_HWSURFACE);
    if(tt_screen == NULL)
    {
        fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Set Window Title */
    SDL_WM_SetCaption(title, NULL);

    /* Make Default Cursor Surface */
    tt_baseCursor = IMG_Load(TURTLE_SPRITE);

    /* Init SDL_ttf */
    if(TTF_Init() == -1)
    {
        fprintf(stderr, "TTF_Init() failed: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    tt_font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
}

/**
 * Only initializes library-specific values. For advanced
 * users only; make sure you have initialized everything else
 * before using this.
 * @param screen
 */
void TT_InitMinimal(SDL_Surface* screen)
{
    tt_screen = screen;
    tt_baseCursor = IMG_Load(TURTLE_SPRITE);
    tt_font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
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
    turt->isFilling = false;
    turt->onclick = NULL; /* sorry for the null pointer */
    turt->onkeyb = NULL;

    turt->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, BITS_PER_PIXEL, 0, 0, 0, 0);
    turt->surfacePos.x = 0;
    turt->surfacePos.y = 0;

    /* TODO find better sprite for cursor? */

    /* Error Control */
    if(turt->surface == NULL)
    {
        fprintf(stderr, "TT_Create() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    turt->color = SDL_MapRGB(turt->surface->format, 255, 255, 255); /* white */
    turt->bgColor = SDL_MapRGB(turt->surface->format, r, g, b);

    /* Init Surface */
    SDL_FillRect(turt->surface, NULL, turt->bgColor);
    /*printf("Base: x=%d y=%d angle=%f\n", turt->x, turt->y, turt->angle);*/

    return turt;
}

/**
 * Waits for user events (mouse click, key press, quit...), then
 * processes these events and redraws the screen (trails + cursor)
 * @param turt
 * @return true until the user closes the window or presses ESC / Q
 */
bool TT_MainLoop(struct Turtle* turt)
{
    /* Check for User Events */
    SDL_Event ev;
    SDL_WaitEvent(&ev);

    /* User Exit */
    if(ev.type == SDL_QUIT)
    {
        return false;
    }

    /* User Keyboard Press */
    if(ev.type == SDL_KEYDOWN)
    {
        SDLKey sym = ev.key.keysym.sym;
        if(turt->onkeyb != NULL)
        {
            turt->onkeyb(sym, ev.key.keysym.mod);
        }
        else if(sym == SDLK_ESCAPE || sym == SDLK_q)
        {
            return false;
        }
    }

    /* User Mouse Click */
    if(ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT
            && turt->onclick != NULL)
    {
        /*printf("Click @ x=%d y=%d\n", ev.button.x, ev.button.y);*/
        turt->onclick(ev.button.x, ev.button.y);
    }

    /* Clear Screen */
    SDL_FillRect(tt_screen, NULL, turt->bgColor);

    /* Paint Turtle */
    TT_Blit(turt);

    /* Refresh Window */
    SDL_Flip(tt_screen);

    return true;
}

/**
 * Blits the turtle once. Use this only if you are managing
 * the main loop yourself.
 * @param turt
 */
void TT_Blit(struct Turtle* turt)
{
    /* Paint Turtle Surface (aka current trails) */
    SDL_BlitSurface(turt->surface, NULL, tt_screen, &(turt->surfacePos));

    /* Paint Cursor as Necessary */
    if(turt->isVisible)
    {
        SDL_Surface* cursor = rotozoomSurface(tt_baseCursor, -abs(turt->angle), 1.0, 1);
        SDL_Rect cursorPos;
        cursorPos.x = (turt->x - cursor->w / 2) + turt->surfacePos.x;
        cursorPos.y = (turt->y - cursor->h / 2) + turt->surfacePos.y;

        SDL_BlitSurface(cursor, NULL, tt_screen, &cursorPos);

        SDL_FreeSurface(cursor);
    }
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
    TTF_CloseFont(tt_font);
    TTF_Quit();
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

void TT_SetSurfacePos(struct Turtle* turt, int x, int y)
{
    turt->surfacePos.x = x;
    turt->surfacePos.y = y;
}

/**
 * Moves the cursor to the specified location
 * @param turt
 * @param x
 * @param y
 */
void TT_MoveTo(struct Turtle* turt, int x, int y)
{
    /* Boundary Check */
    if(x < 0 || y < 0 || x >= turt->surface->w || y >= turt->surface->h)
    {
        fprintf(stderr, "TT_MoveTo: going outside world!\n");
        exit(EXIT_FAILURE);
    }

    /*printf("Destination: x=%d y=%d\n", x, y);*/

    /* Draw Line as Necessary */
    if(turt->isDrawing)
    {
        Draw_Line(turt->surface, turt->x, turt->y, x, y, turt->color);
    }

    /* Move Turtle */
    turt->x = x;
    turt->y = y;

    /* Fill Shape */
    if(turt->isFilling)
    {
        if(turt->fillIndex < turt->fillCount)
        {
            /* Push Vertex */
            turt->fillX[turt->fillIndex] = x;
            turt->fillY[turt->fillIndex] = y;
            /*printf("Push [%d, %d] to vertex array (count=%d; max=%d)\n",
                   x, y, turt->fillIndex, turt->fillCount);*/
            turt->fillIndex ++;
        }

        if(turt->fillIndex >= turt->fillCount)
        {
            /* Do Fill */
            filledPolygonColor(turt->surface, turt->fillX, turt->fillY, turt->fillCount, turt->fillColor);
            /*printf("End fill");*/
            turt->isFilling = false;
            free(turt->fillX);
            free(turt->fillY);
        }
    }
}

/**
 * Rotates the turtle left
 * @param turt
 * @param deg
 */
void TT_Left(struct Turtle* turt, float deg)
{
    turt->angle = mod(turt->angle - deg, 360.0f); /* clockwise */
    /*printf("Angle: %f\n", turt->angle);*/
}

/**
 * Rotates the turtle right
 * @param turt
 * @param deg
 */
void TT_Right(struct Turtle* turt, float deg)
{
    turt->angle = mod(turt->angle + deg, 360.0f); /* counterclockwise */
    /*printf("Angle: %f\n", turt->angle);*/
}

/**
 * Moves the turtle forward
 * @param turt
 * @param distance
 */
void TT_Forward(struct Turtle* turt, int distance)
{
    /* Evaluate Dest Location from Polar Coordinates */
    double offset_x = distance * cos(abs(turt->angle) * RAD2DEG);
    /*printf("offset_x=%f ", offset_x);*/
    double offset_y = distance * sin(abs(turt->angle) * RAD2DEG);
    /*printf("offset_y=%f\n", offset_y);*/

    int x = turt->x + round(offset_x);
    int y = turt->y + round(offset_y);

    /* Boundary Check (X) */
    if(x < 0)
    {
        x = 0;
    }
    else if(x >= turt->surface->w)
    {
        x = turt->surface->w - 1;
    }

    /* Boundary Check (Y) */
    if(y < 0)
    {
        y = 0;
    }
    else if(y >= turt->surface->h)
    {
        y = turt->surface->h - 1;
    }

    TT_MoveTo(turt, x, y);
}

/**
 * Moves the turtle backward
 * @param turt
 * @param distance
 */
void TT_Backward(struct Turtle* turt, int distance)
{
    TT_Forward(turt, -distance);
}

/**
 * Moves the turtle to its origin (without drawing)
 * @param turt
 */
void TT_Home(struct Turtle* turt)
{
    turt->x = turt->surface->w / 2;
    turt->y = turt->surface->h / 2;
    turt->angle = 0.0;
}

/**
 * Clears the drawing surface
 * @param turt
 */
void TT_Clear(struct Turtle* turt)
{
    /* Init Surface */
    SDL_FillRect(turt->surface, NULL, turt->bgColor);
}

/**
 * Clears the drawing surface and sends the turtle home
 * @param turt
 */
void TT_Reset(struct Turtle* turt)
{
    TT_Clear(turt);
    TT_Home(turt);
}

/**
 * Stop drawing
 * @param turt
 */
void TT_PenUp(struct Turtle* turt)
{
    turt->isDrawing = false;
}

/**
 * Start drawing
 * @param turt
 */
void TT_PenDown(struct Turtle* turt)
{
    turt->isDrawing = true;
}

/**
 * Makes the turtle visible
 * @param turt
 */
void TT_ShowTurtle(struct Turtle* turt)
{
    turt->isVisible = true;
}

/**
 * Makes the turtle invisible (better for finished drawings)
 * @param turt
 */
void TT_HideTurtle(struct Turtle* turt)
{
    turt->isVisible = false;
}

/**
 * Writes text at the current position of the turtle
 * @param turt
 * @param str
 */
void TT_WriteText(struct Turtle* turt, const char* str)
{
    SDL_Surface* text = TTF_RenderText_Blended(tt_font, str, translate_color(turt->color));
    SDL_Rect pos;
    pos.x = turt->x;
    pos.y = turt->y;
    SDL_BlitSurface(text, NULL, turt->surface, &pos);
    SDL_FreeSurface(text);
}

void TT_Circle(struct Turtle* turt, int radius)
{
    /* Center is R Units Left to Turtle */
    double offset_x = radius * cos(mod(turt->angle - 90, 360.0f) * RAD2DEG);
    double offset_y = radius * sin(mod(turt->angle - 90, 360.0f) * RAD2DEG);

    int x = turt->x + round(offset_x);
    int y = turt->y + round(offset_y);
    printf("Circle center is x=%d y=%d\n", x, y);

    if(x < 0 || y < 0 || x >= turt->surface->w || y >= turt->surface->h)
    {
        fprintf(stderr, "TT_Circle: circle center is off limits!\n");
        return;
    }

    Draw_Circle(turt->surface, x, y, radius, turt->color);
}

void TT_CenteredCircle(struct Turtle* turt, int radius)
{
    Draw_Circle(turt->surface, turt->x, turt->y, radius, turt->color);
}

/*
 * Event API
 */

/**
 * Add handler function to mouse clicks
 * @param turt
 * @param func the function to be called; takes 2 int parameters
 * representing the X and Y click coordinates, returns nothing
 */
void TT_OnClick(struct Turtle* turt, void (*func)(int, int))
{
    turt->onclick = func;
}

/**
 * Add handler function to keyboard presses
 * @param turt
 * @param func the function to be called; takes 1 SDLKey
 * and 1 SDLMod (for now), returns nothing
 */
void TT_OnKeyb(struct Turtle* turt, void (*func)(SDLKey, SDLMod))
{
    turt->onkeyb = func;
}

/*
 * Fill API
 */

/**
 * Starts filling a region of the drawing surface
 * @param turt
 * @param count number of vertices in the polygon to fill
 * @param r fill color (red component)
 * @param g fill color (green component)
 * @param b fill color (blue component)
 */
void TT_BeginFill(struct Turtle* turt, int count, Uint32 r, Uint32 g, Uint32 b)
{
    /* No Double Call */
    if(turt->isFilling)
    {
        fprintf(stderr, "TT_BeginFill: already filling!\n");
        return;
    }

    /* Init Variables */
    turt->fillCount = count;
    turt->fillColor = (r << 24) | (g << 16) | (b << 8) | 0xFF;
    turt->fillX = malloc(sizeof(int) * count);
    turt->fillY = malloc(sizeof(int) * count);

    /* Error Control */
    if(turt->fillX == NULL || turt->fillY == NULL)
    {
        fprintf(stderr, "TT_BeginFill: malloc failed!\n");
        exit(EXIT_FAILURE);
    }

    /*printf("Begin fill using r=%d g=%d b=%d\n", r, g, b);*/

    /* Push First Vertex */
    turt->fillX[0] = turt->x;
    turt->fillY[0] = turt->y;
    /*printf("Push [%d, %d] to vertex array (count=%d; max=%d)\n",
           turt->x, turt->y, 0, turt->fillCount);*/

    turt->isFilling = true;
    turt->fillIndex = 1;
}
