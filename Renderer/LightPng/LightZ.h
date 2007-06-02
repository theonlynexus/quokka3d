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
// Very small & light Zlib compatible decompressor (deflate/inflate)
 #ifndef __LightZPng__LightZ_h
#define __LightZPng__LightZ_h

// Configure the function name here
#define ZFN(x) LZ_##x

///////////////////////////////

// Disable annoying warning message 4127 "conditional expression is constant"
#ifdef _MSC_VER
#pragma warning( disable : 4127 )
#endif

struct LightZ_Env;

// Allocates a new LightZ environment
LightZ_Env *ZFN(NewEnv)();

// Deallocates a LightZ environment
void ZFN(NewEnv)( LightZ_Env *env );

// Inflates the given source data.
// Params:
//  source        - The source data pointer (non-null if source_len > 0)
//  source_len    - Length of the source data
//  dest          - Pointer to the destination data area.
//                  If the destination needs enlargement, the old pointer is delete []:d away.
//  dest_len      - Pointer to the current destination data area size.
//                  The value will be updated to contain the new unpacked data size.
//  env           - The optional memory environment (do not use the same env concurrently in many calls)
// If everything was ok, return null. Otherwise the return value is the error message.
const char *ZFN(Inflate)(
	const void *source, int source_len,
	unsigned char **dest, int *dest_len,
	LightZ_Env *env = 0 );

///////////////////////////////
// Clean up the header/define mess
#ifndef LIGHTZ_INTERNAL
# undef ZFN
#endif

#endif
