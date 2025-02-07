#include <limits>
#include "primitives.h"

using namespace PixelToaster;

namespace Quokka3D
{
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
