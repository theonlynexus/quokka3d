/** Part of the LightZPng library package, v1.02
 ** 2005-2006 Jere Sanisalo & Housemarque Inc
 **
 ** License for this file (MIT):
 **
 ** Copyright (c) 2005 Jere Sanisalo & Housemarque Inc
 **
 ** Permission is hereby granted, free of charge, to any person obtaining a
 ** copy of this software and associated documentation files (the "Software"),
 ** to deal in the Software without restriction, including without limitation
 ** the rights to use, copy, modify, merge, publish, distribute, sublicense,
 ** and/or sell copies of the Software, and to permit persons to whom the
 ** Software is furnished to do so, subject to the following conditions:
 **
 ** The above copyright notice and this permission notice shall be included in
 ** all copies or substantial portions of the Software.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 ** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 ** DEALINGS IN THE SOFTWARE.
 **/
// Very small & light .png loader
#ifndef __LightZPng__LightPng_h
#define __LightZPng__LightPng_h

// Configure the function/struct name here
#define PNGNAME(x) LPNG_##x

///////////////////////////////

// Disable annoying warning message 4127 "conditional expression is constant"
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif


// One .png image
struct PNGNAME(Image)
{
	typedef unsigned int Color;

	int width;
	int height;
	bool has_palette;

	// ARGB colors; either null or "paletteEntries" entries
	Color *palette;
    // Number of entries in the palette
    int palette_size;

	// The color data; either 8 bit palette indices or ARGB 32bit colors
	unsigned char *data;

	PNGNAME(Image)();
	~PNGNAME(Image)();
};

// The unpacker (LightZ) env
struct LightZ_Env;

// Create a .png image from the given data.
// Returns null on error.
PNGNAME(Image) *PNGNAME(Create)( const void *data, int data_size, LightZ_Env *z_env = 0 );


///////////////////////////////
// Clean up the header/define mess
#ifndef LIGHTPNG_INTERNAL
# undef ZFN
#endif

#endif
