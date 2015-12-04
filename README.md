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
(TODO write better description)

# About MTurt

MTurt is a wrapper library for MTurtle which does not require passing a Turtle struct
to all functions. You can use these functions (Turt* instead of TT_*) if you do not
require more advanced functionality.

# About the MTurtle console

The MTurtle console (consolev2) is an application allowing you to enter commands to control
the on-screen turtle, so you can do the graphics you want without even recompiling anything.

Command list coming soon!

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

