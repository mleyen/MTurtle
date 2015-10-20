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
    int x;                      /* cursor position (X) */
    int y;                      /* cursor position (Y) */
    float angle;                /* cursor orientation */
    bool isDrawing;             /* is the pen up or down? */
    bool isVisible;             /* is the turtle visible or not? */
    Uint32 color;               /* line color */
    Uint32 bgColor;             /* background color */

    Uint32 fillColor;           /* shape fill color */
    Sint16* fillX;              /* shape fill vertex array (X) */
    Sint16* fillY;              /* shape fill vertex array (Y) */
    int fillCount;              /* shape fill vertex count */
    int fillIndex;              /* shape fill current vertex */
    bool isFilling;             /* are we currently filling a polygon? */
    SDL_Surface* surface;       /* SDL surface for the turtle screen */
    void (*onclick)(int, int);  /* click event handler func */
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
 * Check if we should keep running the application, then redraws the
 * screen (blits main surface and cursor)
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

void TT_Backward(struct Turtle* turt, int distance);

void TT_Home(struct Turtle* turt);

void TT_Clear(struct Turtle* turt);

void TT_Reset(struct Turtle* turt);

void TT_PenUp(struct Turtle* turt);

void TT_PenDown(struct Turtle* turt);

void TT_ShowTurtle(struct Turtle* turt);

void TT_HideTurtle(struct Turtle* turt);

void TT_WriteText(struct Turtle* turt, const char* str);

/*
 * Event API
 */

void TT_OnClick(struct Turtle* turt, void (*func)(int, int));

/*
 * Fill API
 */

void TT_BeginFill(struct Turtle* turt, int count, Uint32 r, Uint32 g, Uint32 b);

#endif /* __MTURTLE_H_ */
