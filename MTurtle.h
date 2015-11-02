#ifndef __MTURTLE_H_
#define __MTURTLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "SDL_rotozoom.h"
#include "SDL_draw.h"

/**
 * The Turtle struct. Describes a turtle cursor, with its position,
 * orientation, color, and associated SDL surface.
 */
struct Turtle
{
    int x;                          /* cursor position (X) */
    int y;                          /* cursor position (Y) */
    float angle;                    /* cursor orientation */
    bool isDrawing;                 /* is the pen up or down? */
    bool isVisible;                 /* is the turtle visible or not? */
    Uint32 color;                   /* line color */
    Uint32 bgColor;                 /* background color */

    Uint32 fillColor;               /* shape fill color */
    Sint16* fillX;                  /* shape fill vertex array (X) */
    Sint16* fillY;                  /* shape fill vertex array (Y) */
    int fillCount;                  /* shape fill vertex count */
    int fillIndex;                  /* shape fill current vertex */
    bool isFilling;                 /* are we currently filling a polygon? */
    SDL_Surface* surface;           /* SDL surface for the turtle screen */
    void (*onclick)(int, int);      /* click event handler func */
    void (*onkeyb)(SDLKey, SDLMod); /* keyboard event handler func -- TODO avoid exposing SDL */
};

/**
 * Initializes the MTurtle library. Must be called before any other
 * function.
 * @param title window title
 * @param w window width
 * @param h window height
 */
void TT_Init(const char* title, int w, int h);

/**
 * Creates a new turtle.
 * @param w surface width
 * @param h surface height
 * @param r background color - red component
 * @param g background color - green component
 * @param b background color - blue component
 * @return New Turtle struct
 */
struct Turtle* TT_Create(int w, int h, int r, int g, int b);

/**
 * Waits for the user to close the window or press ESC / Q.
 */
void TT_WaitUserExit();

/**
 * Waits for user events (mouse click, key press, quit...), then
 * processes these events and redraws the screen (trails + cursor)
 * @param turt
 * @return true until the user closes the window or presses ESC / Q
 */
bool TT_MainLoop(struct Turtle* turt);

/**
 * Destroys a Turtle struct.
 * @param turt the victim
 */
void TT_Destroy(struct Turtle* turt);

/**
 * Uninitializes the SDL. Call this at the end of your program, before
 * `return EXIT_SUCCESS'.
 */
void TT_EndProgram();

/**
 * Sets the line color for the given turtle
 * @param turt
 * @param r
 * @param g
 * @param b
 */
void TT_SetColor(struct Turtle* turt, Uint32 r, Uint32 g, Uint32 b);

/**
 * Moves the cursor to the specified location
 * @param turt
 * @param x
 * @param y
 */
void TT_MoveTo(struct Turtle* turt, int x, int y);

/**
 * Rotates the turtle left
 * @param turt
 * @param deg
 */
void TT_Left(struct Turtle* turt, float deg);

/**
 * Rotates the turtle right
 * @param turt
 * @param deg
 */
void TT_Right(struct Turtle* turt, float deg);

/**
 * Moves the turtle forward
 * @param turt
 * @param distance
 */
void TT_Forward(struct Turtle* turt, int distance);

/**
 * Moves the turtle backward
 * @param turt
 * @param distance
 */
void TT_Backward(struct Turtle* turt, int distance);

/**
 * Moves the turtle to its origin (without drawing)
 * @param turt
 */
void TT_Home(struct Turtle* turt);

/**
 * Clears the drawing surface
 * @param turt
 */
void TT_Clear(struct Turtle* turt);

/**
 * Clears the drawing surface and sends the turtle home
 * @param turt
 */
void TT_Reset(struct Turtle* turt);

/**
 * Stop drawing
 * @param turt
 */
void TT_PenUp(struct Turtle* turt);

/**
 * Start drawing
 * @param turt
 */
void TT_PenDown(struct Turtle* turt);

/**
 * Makes the turtle visible
 * @param turt
 */
void TT_ShowTurtle(struct Turtle* turt);

/**
 * Makes the turtle invisible (better for finished drawings)
 * @param turt
 */
void TT_HideTurtle(struct Turtle* turt);

/**
 * Writes text at the current position of the turtle
 * @param turt
 * @param str
 */
void TT_WriteText(struct Turtle* turt, const char* str);

void TT_Circle(struct Turtle* turt, int radius);

void TT_CenteredCircle(struct Turtle* turt, int radius);

/*
 * Event API
 */

/**
 * Add handler function to mouse clicks
 * @param turt
 * @param func the function to be called; takes 2 int parameters
 * representing the X and Y click coordinates, returns nothing
 */
void TT_OnClick(struct Turtle* turt, void (*func)(int, int));

/**
 * Add handler function to keyboard presses
 * @param turt
 * @param func the function to be called; takes 1 SDLKey
 * and 1 SDLMod (for now), returns nothing
 */
void TT_OnKeyb(struct Turtle* turt, void (*func)(SDLKey, SDLMod));

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
void TT_BeginFill(struct Turtle* turt, int count, Uint32 r, Uint32 g, Uint32 b);

#endif /* __MTURTLE_H_ */
