#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "PixelToaster.h"

extern std::vector<PixelToaster::TrueColorPixel> pixels;  // 
extern const int width;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

namespace Quokka3D 
{
    #define sgn(x) ((x<0)?-1:((x>0)?1:0))       // macro to return the sign of a number
    typedef unsigned int TRUECOLOR;             // 32-bit color as argb

    void cls();
    void line_fast(int x1, int y1, int x2, int y2, TRUECOLOR color);
    void line_fast(int x1, int x2, int y, unsigned int color);
    void plot_pixel(int x,int y, TRUECOLOR color);

}

#endif  //PRIMITIVES_H