#include <limits>
#include "primitives.h"

using namespace PixelToaster;

namespace Soft3D
{

    void cls()
    {
        memset((void*)&pixels[0], 0, pixels.size()*4); 

        /*for(vector<TrueColorPixel>::iterator i = pixels.begin(); i != pixels.end(); ++i)
            i->integer = (DWORD)0;*/
    }

    // Plot pixel using PixelToaster in TrueColor mode
    void plot_pixel(int x,int y, TRUECOLOR color)
    {   
      //  PixelToaster video memory is laid out linearly, so to plot [x,y] we 
      //  can use the formula: pixels(y * x_res + x). Eg To speed up the plot we can use shifts
      //  instead of multiplication for common resolutions:
      //  y*320  = y*256 + y*64  = y*2^8 + y*2^6
      //  y*640  = y*512 + y*128 = y*2^9 + y*2^7
      //  y*800  = y*512 + y*256 + y*32 = y*2^9 + y*2^8 + y*2^5
      //  y*1024 = y*2^10

      //  pixels[(y<<8)+(y<<6)+x].integer = color;  // 320x200
      //  pixels[(y<<9)+(y<<7)+x].integer = color;  // 640x480
      //  pixels[(y<<9)+(y<<8)+(y<<5)+x].integer = color;  // 800x600
      //  pixels[(y<<10)+x].integer = color;  // 1024x768
        
        // if this is slow, try one of the above lines
        pixels[y*width+x].integer = color;
         
    }

    /**************************************************************************
     *    line_fast                                                           *
     *    draws a line using Bresenham's line-drawing algorithm, which uses   *
     *    no multiplication or division. Original code by David Brackeen.     *
     *    Color is represented as 32-bit xrgb.                                *
     **************************************************************************/
    void line_fast(int x1, int y1, int x2, int y2, unsigned int color)
    {
        int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

        dx = x2 - x1;      /* the horizontal distance of the line */
        dy = y2 - y1;      /* the vertical distance of the line */
        dxabs = abs(dx);
        dyabs = abs(dy);
        sdx = sgn(dx);
        sdy = sgn(dy);
        x = dyabs >> 1;
        y = dxabs >> 1;
        px = x1;
        py = y1;

        plot_pixel(px, py, color);

        if (dxabs >= dyabs) /* the line is more horizontal than vertical */
        {
            for(i=0; i<dxabs; i++)
            {
                y += dyabs;
                if (y >= dxabs)
                {
                    y -= dxabs;
                    py += sdy;
                }
                px += sdx;
                plot_pixel(px, py, color);
            }
        }
        else /* the line is more vertical than horizontal */
        {
            for(i=0; i<dyabs; i++)
            {
                x += dxabs;
                if (x >= dyabs)
                {
                    x -= dyabs;
                    px += sdx;
                }
                py += sdy;
                plot_pixel(px, py, color);
            }
        }
    }
} //Soft3D
