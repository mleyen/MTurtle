#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "SDL_rotozoom.h"
#include "SDL_draw.h"
#include "MTurtle.h"

#define BITS_PER_PIXEL 32
#define PI 3.14159265
#define RAD2DEG PI / 180.0

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
    tt_baseCursor = IMG_Load("triangle3.png");

    /* Init SDL_ttf */
    if(TTF_Init() == -1)
    {
        fprintf(stderr, "TTF_Init() failed: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    tt_font = TTF_OpenFont("trisk.ttf", 13);
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

    turt->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, BITS_PER_PIXEL, 0, 0, 0, 0);
    /*turt->cursorSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32, 0, 0, 0, 0);*/
    /*turt->cursorSurface = rotozoomSurface(tt_baseCursor, angle, 1.0, 1);*/ /* can be done in the main loop? */

    /* TODO find better sprite for cursor? */

    /* Error Control */
    if(turt->surface == NULL /*|| turt->cursorSurface == NULL*/)
    {
        fprintf(stderr, "TT_Create() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    turt->color = SDL_MapRGB(turt->surface->format, 255, 255, 255); /* white */
    turt->cursorColor = SDL_MapRGB(turt->surface->format, 255, 0, 0); /* red */
    turt->bgColor = SDL_MapRGB(turt->surface->format, r, g, b);

    /* Init Surface */
    SDL_FillRect(turt->surface, NULL, turt->bgColor);
    /*printf("Base: x=%d y=%d angle=%f\n", turt->x, turt->y, turt->angle);*/

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
 * Check if we should keep running the application, then redraws the
 * screen (blits main surface and cursor)
 * @return true until the user closes the window or presses ESC / Q
 */
bool TT_MainLoop(struct Turtle* turt)
{
    /* Check for User Exit */
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

    /* Clear Screen */
    SDL_FillRect(tt_screen, NULL, turt->bgColor);

    /* Paint Turtle Surface (aka current trails) */
    SDL_Rect surfacePos;
    surfacePos.x = 0;
    surfacePos.y = 0;
    SDL_BlitSurface(turt->surface, NULL, tt_screen, &surfacePos);

    /* Paint Cursor as Necessary */
    if(turt->isVisible)
    {
        SDL_Surface* cursor = rotozoomSurface(tt_baseCursor, -abs(turt->angle), 1.0, 1);
        SDL_Rect cursorPos;
        cursorPos.x = turt->x - cursor->w / 2;
        cursorPos.y = turt->y - cursor->h / 2;

        SDL_BlitSurface(cursor, NULL, tt_screen, &cursorPos);

        SDL_FreeSurface(cursor);
    }

    /* Refresh Window */
    SDL_Flip(tt_screen);

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

/**
 * Moves the cursor to the specified location
 * @param turt
 * @param x
 * @param y
 */
void TT_MoveTo(struct Turtle* turt, int x, int y)
{
    /*printf("Destination: x=%d y=%d\n", x, y);*/

    /* Draw Line as Necessary */
    if(turt->isDrawing)
    {
        Draw_Line(turt->surface, turt->x, turt->y, x, y, turt->color);
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
    turt->angle = fmod(turt->angle + deg, 360.0f); /* counterclockwise */
    /*printf("Angle: %f\n", turt->angle);*/
}

/**
 * Rotates the turtle right
 * @param turt
 * @param deg
 */
void TT_Right(struct Turtle* turt, float deg)
{
    turt->angle = fmod(turt->angle - deg, 360.0f); /* clockwise */
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

    TT_MoveTo(turt, x, y);
}

void TT_Backward(struct Turtle* turt, int distance)
{
    TT_Forward(turt, -distance);
}

void TT_Home(struct Turtle* turt)
{
    turt->x = turt->surface->w / 2;
    turt->y = turt->surface->h / 2;
    turt->angle = 0.0;
}

void TT_Clear(struct Turtle* turt)
{
    /* Init Surface */
    SDL_FillRect(turt->surface, NULL, turt->bgColor);
}

void TT_Reset(struct Turtle* turt)
{
    TT_Clear(turt);
    TT_Home(turt);
}

void TT_PenUp(struct Turtle* turt)
{
    turt->isDrawing = false;
}

void TT_PenDown(struct Turtle* turt)
{
    turt->isDrawing = true;
}

void TT_ShowTurtle(struct Turtle* turt)
{
    turt->isVisible = true;
}

void TT_HideTurtle(struct Turtle* turt)
{
    turt->isVisible = false;
}

void TT_WriteText(struct Turtle* turt, const char* str)
{
    SDL_Surface* text = TTF_RenderText_Blended(tt_font, str, translate_color(turt->color));
    SDL_Rect pos;
    pos.x = turt->x;
    pos.y = turt->y;
    SDL_BlitSurface(text, NULL, turt->surface, &pos);
    SDL_FreeSurface(text);
}
