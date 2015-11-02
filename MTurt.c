/*
 * Simpler version of the library, doesn't require a struct Turtle
 * pointer in the code, only allows one turtle per program.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include "MTurtle.h"
#include "MTurt.h"

/**
 * Global Turtle struct
 */
struct Turtle* turt;

/**
 * Initialize library and create turtle singleton
 * @param title
 * @param w
 * @param h
 * @param r
 * @param g
 * @param b
 */
void TurtInit(const char* title, int w, int h, int r, int g, int b)
{
    TT_Init(title, w, h);
    turt = TT_Create(w, h, r, g, b);
}

/**
 * Waits for user events (mouse click, key press, quit...), then
 * processes these events and redraws the screen (trails + cursor)
 * @return true until the user closes the window or presses ESC / Q
 */
bool TurtMainLoop()
{
    return TT_MainLoop(turt);
}

/**
 * Exit program
 * @param status shell return status (usually EXIT_SUCCESS)
 */
void TurtExit(int status)
{
    TT_Destroy(turt);
    TT_EndProgram();
    exit(status);
}

/**
 * Sets the line color for the given turtle
 * @param r
 * @param g
 * @param b
 */
void TurtSetColor(Uint32 r, Uint32 g, Uint32 b)
{
    TT_SetColor(turt, r, g, b);
}

/**
 * Moves the cursor to the specified location
 * @param x
 * @param y
 */
void TurtMoveTo(int x, int y)
{
    TT_MoveTo(turt, x, y);
}

/**
 * Rotates the turtle left
 * @param deg
 */
void TurtLeft(float deg)
{
    TT_Left(turt, deg);
}

/**
 * Rotates the turtle right
 * @param deg
 */
void TurtRight(float deg)
{
    TT_Right(turt, deg);
}

/**
 * Moves the turtle forward
 * @param distance
 */
void TurtForward(int distance)
{
    TT_Forward(turt, distance);
}

/**
 * Moves the turtle backward
 * @param distance
 */
void TurtBackward(int distance)
{
    TT_Backward(turt, distance);
}

/**
 * Moves the turtle to its origin (without drawing)
 */
void TurtHome()
{
    TT_Home(turt);
}

/**
 * Clears the drawing surface
 */
void TurtClear()
{
    TT_Clear(turt);
}

/**
 * Clears the drawing surface and sends the turtle home
 */
void TurtReset()
{
    TT_Reset(turt);
}

/**
 * Stop drawing
 */
void TurtPenUp()
{
    TT_PenUp(turt);
}

/**
 * Start drawing
 */
void TurtPenDown()
{
    TT_PenDown(turt);
}

/**
 * Makes the turtle visible
 */
void TurtShow()
{
    TT_ShowTurtle(turt);
}

/**
 * Makes the turtle invisible (better for finished drawings)
 */
void TurtHide()
{
    TT_HideTurtle(turt);
}

/**
 * Writes text at the current position of the turtle
 * @param str
 */
void TurtWriteText(const char* str)
{
    TT_WriteText(turt, str);
}

void TurtCircle(int radius)
{
    TT_Circle(turt, radius);
}

void TurtCenteredCircle(int radius)
{
    TT_CenteredCircle(turt, radius);
}

/*
 * Event API
 */

/**
 * Add handler function to mouse clicks
 * @param func the function to be called; takes 2 int parameters
 * representing the X and Y click coordinates, returns nothing
 */
void TurtOnClick(void (*func)(int, int))
{
    TT_OnClick(turt, func);
}

/**
 * Add handler function to keyboard presses
 * @param func the function to be called; takes 1 SDLKey
 * and 1 SDLMod (for now), returns nothing
 */
void TurtOnKeyb(void (*func)(SDLKey, SDLMod))
{
    TT_OnKeyb(turt, func);
}

/*
 * Fill API
 */

/**
 * Starts filling a region of the drawing surface
 * @param count number of vertices in the polygon to fill
 * @param r fill color (red component)
 * @param g fill color (green component)
 * @param b fill color (blue component)
 */
void TurtBeginFill(int count, Uint32 r, Uint32 g, Uint32 b)
{
    TT_BeginFill(turt, count, r, g, b);
}
