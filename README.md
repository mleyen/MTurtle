# About MTurtle

MTurtle is a C library for simple development of
[turtle graphics](https://en.wikipedia.org/wiki/Turtle_graphics) applications.

It is meant to be used primarily for educational purposes.

MTurtle uses the SDL library to perform graphical operations and rendering.

# How to install

Before you install MTurtle, you need the following packages:
* SDL (not SDL2)
* SDL_image
* SDL_ttf
* [SDL_draw](http://sdl-draw.sourceforge.net/)
* [SDL_gfx](http://cms.ferzkopp.net/index.php/software/13-sdl-gfx)
* [SDL_terminal](http://sourceforge.net/projects/sdl-terminal/)

If you want to install the MTurtle console, you also need:
* flex
* bison

To compile the library and all examples, run:

```bash
make
```

To compile the MTurtle console, run:

```bash
make consolev2
```

You can put the resulting executables in any directory of your liking.

# How to use

Check out MTurtle.h for a brief summary of the functions provided.

To use these functions, simply include the MTurtle.h header in your C source file.

The TT_Init() function must be called first, followed by TT_Create.
`TT_Init(const char* title, int w, int h)` takes as arguments the title of the window,
and its dimensions.

`TT_Create(int w, int h, int r, int g, int b)` takes the dimensions of the drawing area
(w and h) and its background color (r, g and b, ranging from 0 to 255), and returns a
`struct Turtle*`.

All other functions take a `struct Turtle*` as an argument.

When you're done drawing, just use `while(TT_MainLoop(turt))` to keep the window open.
This will also process mouse and keyboard events if you defined them.

See the example files hello.c and spirale.c for details.

## Events

If you wish to have a function called every time the user clicks on the drawing area
or presses a key, use the TT_OnClick and TT_OnKeyb functions to assign callbacks to these
functions. See the draw.c file for an example.

The function called by TT_OnClick takes 2 arguments, which are the coordinates of the click.
The function called by TT_OnKeyb takes 2 arguments: the key pressed and its modifiers
(Ctrl, Alt...), as SDLKey and SDLMod types, respectively.
See the [SDL documentation](https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlkey.html)
for details.

# About MTurt

MTurt is a wrapper library for MTurtle which does not require passing a Turtle struct
to all functions. You can use these functions (Turt* instead of TT_*) if you do not
require more advanced functionality.

To use these functions, include the MTurt.h header.

# About the MTurtle console

The MTurtle console (consolev2) is an application allowing you to enter commands to control
the on-screen turtle, so you can do the graphics you want without even recompiling anything.

The available commands are:
* exit: quit application
* help: show basic help
* fwd n: move forward from n pixels
* back n: move backward from n pixels
* left x: turn x degrees to the left
* right x: turn x degress to the right
* pendown: start drawing
* penup: start moving without drawing
* write x: put x as text at the turtle's current position
* hideturtle: hide cursor
* showturtle: show cursor
* circ n: draw a circle of radius n touching the turtle (center will be left to the turtle)
* centcirc n: draw a circle of radius n centered on the turtle
* home: sends the turtle to the center of the screen
* clear: erases the drawing area
* reset: does both home and clear
* color r g b: change the drawing color to the selected RGB triplet
* color x: change the drawing color to a named color
  * Available colors are red, green, blue, yellow, teal, magenta, orange, black, white, gray
    and silver
* load: loads an external script file

You can execute multiple instructions by separating them with `;`.
In external scripts, you can also use carriage returns to separate instructions.
Note that a list of more than one instruction MUST end with a `;` or new line.

## Expressions

The parameters for fwd, back, left and right, are expressions.

Expressions can be:
* a number (integer or real)
* a variable (see below)
* a mathematical operation, such as `a + b`
  * The +, -, * and / operators are valid
  * The `rmdr` operator can also be used to return the remainder of an Euclidean division
* a mathematical functions, such as `(sqrt x)`
  * The parentheses are required
  * Available functions: cos, sin, tan, abs, sqrt, log, log10, exp, ceil, floor
  * max and min takes 2 arguments: `(max x y)`
* an user-defined function (see below)

## Variables

Variables can be used to keep a value into memory.
To assign a value to a variable:

`x <- 25`

This will assign the value 25 to the variable x.

To display the value of a variable:
`echo x`

The value of the variable can be an expression; e.g.: `i <- i + 1`

## Conditionals

To do different operations based of the value of an expression, you can use
the if/then/else syntax.

`if a == b then fwd 20; left; else back 20; right; endif;`

This will compare a to b, and if they are equal, will execute instructions between `then`
and `else`; otherwise, will execute instructions between `else` and `endif`.

`==`, `!=`, `>`, `<`, `>=` and `<=` can be used as comparison operators.

You can combine comparisons using one of these (note that the parentheses are mandatory):
* `(a == b and c == d)` (both conditions must be true)
* `(a == b or c == d)` (at least one must be true)

## Loops

To execute instructions repeatedly:

`while i < 10 do fwd 20; left; i <- i + 1; endwhile;`

This will execute `fwd 20; left; i <- i + 1;` while i is less than 10.

Alternate syntax:

`for i from 1 to 10 do fwd i; left; endfor;`

To repeat an instruction (or sequence of instructions) N times:

`repeat 4 times fwd 20; left; endrepeat;`

## Functions

You can define functions for common processes. Like so:

    func f(x, y)
        fwd x
        left y
    endfunc
    
To call: `f(20, 45)`

Functions can have return values so they can be used as expressions:

    func g(x, y)
        return x + y
    endfunc
    
    var <- g(t, (sqrt t))

# Licence

MTurtle is released under the GNU General Public Licence. See the COPYING file for more info.

MTurtle is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Uses the SDL 1.2 library. SDL 1.2 is released under the GNU Lesser General Public Licence. See
the file COPYING.LESSER for more info.

The following libraries are released under the [zlib](http://directory.fsf.org/wiki/License:Zlib) licence:
* SDL_image
* SDL_ttf
* SDL_gfx

The following libraries are released under the GNU General Public Licence:
* SDL_draw
* SDL_terminal
* flex
* bison

For technical reasons, duplicates of headers from SDL_draw and SDL_gfx are included with this release.
They have been slightly altered to #include libraries from the default include path rather than the current
directory. Nothing else has been changed.

The example programs (hello.c, lantern.c, spirale.c, draw.c and olympics.c) and .turt scripts (hello.turt,
spirale.turt and fractal.turt) are hereby released under the public domain.

The MiscFixed font is available under the public domain.

Copying and distribution of this file, with or without modification, are permitted in any medium without
royalty provided the copyright notice and this notice are preserved.  This file is offered as-is,
without any warranty.

