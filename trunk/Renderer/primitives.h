#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "PixelToaster.h"

extern std::vector<PixelToaster::TrueColorPixel> pixels; 
//extern PixelToaster::TrueColorPixel pixels[]; 
extern const int width;
extern const int height;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#define MAKE_RGB32(r, g, b) ((((BYTE)(b)|((WORD)((BYTE)(g))<< 8 ))|(((DWORD)(BYTE)(r))<< 16 )))
#define PITCH 4 // PixelToaster works with a 32-bit buffer

namespace Quokka3D 
{
    #define sgn(x) ((x<0)?-1:((x>0)?1:0))       // macro to return the sign of a number
    typedef unsigned int TRUECOLOR;             // 32-bit color as argb

    inline void cls();
    inline void line_horiz(int x1, int x2, int y, unsigned int color);
    inline void plot_pixel(int x,int y, TRUECOLOR color);
    void line_fast(int x1, int y1, int x2, int y2, TRUECOLOR color);

    inline void cls()
    {
        memset((void*)&pixels[0], 0, pixels.size() << 2); 
        //memset((void*)&pixels[0], 0, sizeof pixels); 
    }


    // Plot pixel using PixelToaster in TrueColor mode
    inline void plot_pixel(int x,int y, TRUECOLOR color)
    {   
        //  PixelToaster video memory is laid out linearly, so to plot [x,y] we 
        //  can use the formula: pixels(y * x_res + x). To speed up the plot we can use shifts
        //  instead of multiplication for common resolutions:
        //  y*320  = y*256 + y*64  = y*2^8 + y*2^6
        //  y*640  = y*512 + y*128 = y*2^9 + y*2^7
        //  y*800  = y*512 + y*256 + y*32 = y*2^9 + y*2^8 + y*2^5
        //  y*1024 = y*2^10

        //switch(width)
        //{
        //case 640:
        //    pixels[(y<<9) + (y<<7) + x].integer = color;
        //	break;
        //case 800:
        //    pixels[(y<<9) + (y<<8) + (y<<5)+x].integer = color;
        //	break;
        //case 1024:
        //    pixels[(y<<10) + x].integer = color;
        //    break;
        //case 320:
        //    pixels[(y<<8) + (y<<6) +x].integer = color;
        //    break;
        //default:
        //    break;
        //}

        // Or, be simple and use this
        pixels[y*width+x].integer = color;

    }

    // Draw a horizontal line
    inline void line_horiz(int x1, int x2, int y, unsigned int color)
    {
        for (int x=x1; x<=x2; ++x)
        {
            plot_pixel(x, y, color);
        }
    }


}

#endif  //PRIMITIVES_H