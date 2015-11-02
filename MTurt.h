#ifndef __MTURT_H_
#define __MTURT_H_

#include <stdbool.h>
#include <SDL/SDL.h>

/**
 * Initialize library and create turtle singleton
 * @param title
 * @param w
 * @param h
 * @param r
 * @param g
 * @param b
 */
void TurtInit(const char* title, int w, int h, int r, int g, int b);

/**
 * Waits for user events (mouse click, key press, quit...), then
 * processes these events and redraws the screen (trails + cursor)
 * @return true until the user closes the window or presses ESC / Q
 */
bool TurtMainLoop();

/**
 * Exit program
 * @param status shell return status (usually EXIT_SUCCESS)
 */
void TurtExit(int status);

/**
 * Sets the line color for the given turtle
 * @param r
 * @param g
 * @param b
 */
void TurtSetColor(Uint32 r, Uint32 g, Uint32 b);

/**
 * Moves the cursor to the specified location
 * @param x
 * @param y
 */
void TurtMoveTo(int x, int y);

/**
 * Rotates the turtle left
 * @param deg
 */
void TurtLeft(float deg);

/**
 * Rotates the turtle right
 * @param deg
 */
void TurtRight(float deg);

/**
 * Moves the turtle forward
 * @param distance
 */
void TurtForward(int distance);

/**
 * Moves the turtle backward
 * @param distance
 */
void TurtBackward(int distance);

/**
 * Moves the turtle to its origin (without drawing)
 */
void TurtHome();

/**
 * Clears the drawing surface
 */
void TurtClear();

/**
 * Clears the drawing surface and sends the turtle home
 */
void TurtReset();

/**
 * Stop drawing
 */
void TurtPenUp();

/**
 * Start drawing
 */
void TurtPenDown();

/**
 * Makes the turtle visible
 */
void TurtShow();

/**
 * Makes the turtle invisible (better for finished drawings)
 */
void TurtHide();

/**
 * Writes text at the current position of the turtle
 * @param str
 */
void TurtWriteText(const char* str);

void TurtCircle(int radius);

void TurtCenteredCircle(int radius);

/*
 * Event API
 */

/**
 * Add handler function to mouse clicks
 * @param func the function to be called; takes 2 int parameters
 * representing the X and Y click coordinates, returns nothing
 */
void TurtOnClick(void (*func)(int, int));

/**
 * Add handler function to keyboard presses
 * @param func the function to be called; takes 1 SDLKey
 * and 1 SDLMod (for now), returns nothing
 */
void TurtOnKeyb(void (*func)(SDLKey, SDLMod));

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
void TurtBeginFill(int count, Uint32 r, Uint32 g, Uint32 b);

#endif /* __MTURT_H_ */
