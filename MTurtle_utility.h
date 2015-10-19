#ifndef __MTURTLE_UTILITY_H_
#define __MTURTLE_UTILITY_H_
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

#endif /* __MTURTLE_UTILITY_H_ */
