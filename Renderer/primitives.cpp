#include <limits>
#include "primitives.h"

namespace Quokka3D
{

    void plot_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
    {
        int bpp = surface->format->BytesPerPixel;
        /* Here p is the address to the pixel we want to set */
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

        switch(bpp) 
        {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
        }
    }    



    void line8(SDL_Surface *s, 
        int x1, int y1, 
        int x2, int y2, 
        Uint32 color)
    {
        int d;
        int x;
        int y;
        int ax;
        int ay;
        int sx;
        int sy;
        int dx;
        int dy;

        Uint8 *lineAddr;
        Sint32 yOffset;

        dx = x2 - x1;  
        ax = abs(dx) << 1;  
        sx = sign(dx);

        dy = y2 - y1;  
        ay = abs(dy) << 1;  
        sy = sign(dy);
        yOffset = sy * m_pScreen->pitch;

        x = x1;
        y = y1;

        lineAddr = ((Uint8 *)(m_pScreen->pixels)) + (y * m_pScreen->pitch);
        if (ax>ay)
        {                      /* x dominant */
            d = ay - (ax >> 1);
            for (;;)
            {
                *(lineAddr + x) = (Uint8)color;

                if (x == x2)
                {
                    return;
                }
                if (d>=0)
                {
                    y += sy;
                    lineAddr += yOffset;
                    d -= ax;
                }
                x += sx;
                d += ay;
            }
        }
        else
        {                      /* y dominant */
            d = ax - (ay >> 1);
            for (;;)
            {
                *(lineAddr + x) = (Uint8)color;

                if (y == y2)
                {
                    return;
                }
                if (d>=0) 
                {
                    x += sx;
                    d -= ay;
                }
                y += sy;
                lineAddr += yOffset;
                d += ax;
            }
        }
    }

    //----------------------------------------------------------

    // Draw lines in 16 bit surfaces. Note that this code will
    // also work on 15 bit surfaces.

    void line16(SDL_Surface *s, 
        int x1, int y1, 
        int x2, int y2, 
        Uint32 color)
    {
        int d;
        int x;
        int y;
        int ax;
        int ay;
        int sx;
        int sy;
        int dx;
        int dy;

        Uint8 *lineAddr;
        Sint32 yOffset;

        dx = x2 - x1;  
        ax = abs(dx) << 1;  
        sx = sign(dx);

        dy = y2 - y1;  
        ay = abs(dy) << 1;  
        sy = sign(dy);
        yOffset = sy * m_pScreen->pitch;

        x = x1;
        y = y1;

        lineAddr = ((Uint8 *)m_pScreen->pixels) + (y * m_pScreen->pitch);
        if (ax>ay)
        {                      /* x dominant */
            d = ay - (ax >> 1);
            for (;;)
            {
                *((Uint16 *)(lineAddr + (x << 1))) = (Uint16)color;

                if (x == x2)
                {
                    return;
                }
                if (d>=0)
                {
                    y += sy;
                    lineAddr += yOffset;
                    d -= ax;
                }
                x += sx;
                d += ay;
            }
        }
        else
        {                      /* y dominant */
            d = ax - (ay >> 1);
            for (;;)
            {
                *((Uint16 *)(lineAddr + (x << 1))) = (Uint16)color;

                if (y == y2)
                {
                    return;
                }
                if (d>=0) 
                {
                    x += sx;
                    d -= ay;
                }
                y += sy;
                lineAddr += yOffset;
                d += ax;
            }
        }
    }

    //----------------------------------------------------------

    // Draw lines in 24 bit surfaces. 24 bit surfaces require
    // special handling because the pixels don't fall on even
    // address boundaries. Instead of being able to store a
    // single byte, word, or long you have to store 3
    // individual bytes. As a result 24 bit graphics is slower
    // than the other pixel sizes.

    void line24(SDL_Surface *s, 
        int x1, int y1, 
        int x2, int y2, 
        Uint32 color)
    {
        int d;
        int x;
        int y;
        int ax;
        int ay;
        int sx;
        int sy;
        int dx;
        int dy;

        Uint8 *lineAddr;
        Sint32 yOffset;

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        color <<= 8;
#endif

        dx = x2 - x1;  
        ax = abs(dx) << 1;  
        sx = sign(dx);

        dy = y2 - y1;  
        ay = abs(dy) << 1;  
        sy = sign(dy);
        yOffset = sy * m_pScreen->pitch;

        x = x1;
        y = y1;

        lineAddr = ((Uint8 *)(m_pScreen->pixels)) + (y * m_pScreen->pitch);
        if (ax>ay)
        {                      /* x dominant */
            d = ay - (ax >> 1);
            for (;;)
            {
                Uint8 *p = (lineAddr + (x * 3));
                memcpy(p, &color, 3);

                if (x == x2)
                {
                    return;
                }
                if (d>=0)
                {
                    y += sy;
                    lineAddr += yOffset;
                    d -= ax;
                }
                x += sx;
                d += ay;
            }
        }
        else
        {                      /* y dominant */
            d = ax - (ay >> 1);
            for (;;)
            {
                Uint8 *p = (lineAddr + (x * 3));
                memcpy(p, &color, 3);

                if (y == y2)
                {
                    return;
                }
                if (d>=0) 
                {
                    x += sx;
                    d -= ay;
                }
                y += sy;
                lineAddr += yOffset;
                d += ax;
            }
        }
    }

    //----------------------------------------------------------

    // Draw lines in 32 bit surfaces. Note that this routine
    // ignores alpha values. It writes them into the surface
    // if they are included in the pixel, but does nothing
    // else with them.

    void line32(SDL_Surface *s, 
        int x1, int y1, 
        int x2, int y2, 
        Uint32 color)
    {
        int d;
        int x;
        int y;
        int ax;
        int ay;
        int sx;
        int sy;
        int dx;
        int dy;

        Uint8 *lineAddr;
        Sint32 yOffset;

        dx = x2 - x1;  
        ax = abs(dx) << 1;  
        sx = sign(dx);

        dy = y2 - y1;  
        ay = abs(dy) << 1;  
        sy = sign(dy);
        yOffset = sy * m_pScreen->pitch;

        x = x1;
        y = y1;

        lineAddr = ((Uint8 *)(m_pScreen->pixels)) + (y * m_pScreen->pitch);
        if (ax>ay)
        {                      /* x dominant */
            d = ay - (ax >> 1);
            for (;;)
            {
                *((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

                if (x == x2)
                {
                    return;
                }
                if (d>=0)
                {
                    y += sy;
                    lineAddr += yOffset;
                    d -= ax;
                }
                x += sx;
                d += ay;
            }
        }
        else
        {                      /* y dominant */
            d = ax - (ay >> 1);
            for (;;)
            {
                *((Uint32 *)(lineAddr + (x << 2))) = (Uint32)color;

                if (y == y2)
                {
                    return;
                }
                if (d>=0) 
                {
                    x += sx;
                    d -= ay;
                }
                y += sy;
                lineAddr += yOffset;
                d += ax;
            }
        }
    }

    
    //----------------------------------------------------------

    // Examine the depth of a surface and select a line
    // drawing routine optimized for the bytes/pixel of the
    // surface.

    void line(SDL_Surface *s, 
        int x1, int y1, 
        int x2, int y2, 
        Uint32 color)
    {
        switch (m_pScreen->format->BytesPerPixel)
        {
        case 1:
            line8(s, x1, y1, x2, y2, color);
            break;
        case 2:
            line16(s, x1, y1, x2, y2, color);
            break;
        case 3:
            line24(s, x1, y1, x2, y2, color);
            break;
        case 4:
            line32(s, x1, y1, x2, y2, color);
            break;
        }
    }


    /**************************************************************************
     *    line_fast                                                           *
     *    draws a line using Bresenham's line-drawing algorithm, which uses   *
     *    no multiplication or division. Original code by David Brackeen.     *
     *    Color is represented as 32-bit xrgb.                                *
     **************************************************************************/
    //void line_fast(int x1, int y1, int x2, int y2, unsigned int color)
    //{
    //    int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

    //    dx = x2 - x1;      /* the horizontal distance of the line */
    //    dy = y2 - y1;      /* the vertical distance of the line */
    //    dxabs = abs(dx);
    //    dyabs = abs(dy);
    //    sdx = sgn(dx);
    //    sdy = sgn(dy);
    //    x = dyabs >> 1;
    //    y = dxabs >> 1;
    //    px = x1;
    //    py = y1;

    //    plot_pixel(px, py, color);

    //    if (dxabs >= dyabs) /* the line is more horizontal than vertical */
    //    {
    //        for(i=0; i<dxabs; i++)
    //        {
    //            y += dyabs;
    //            if (y >= dxabs)
    //            {
    //                y -= dxabs;
    //                py += sdy;
    //            }
    //            px += sdx;
    //            plot_pixel(px, py, color);
    //        }
    //    }
    //    else /* the line is more vertical than horizontal */
    //    {
    //        for(i=0; i<dyabs; i++)
    //        {
    //            x += dxabs;
    //            if (x >= dyabs)
    //            {
    //                x -= dyabs;
    //                px += sdx;
    //            }
    //            py += sdy;
    //            plot_pixel(px, py, color);
    //        }
    //    }
    //}
} //Soft3D
