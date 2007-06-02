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
#define LIGHTPNG_INTERNAL
#define LIGHTZ_INTERNAL
#include "LightPng.h"
#include "LightZ.h"
//#include <stdio.h>

#ifdef _MSC_VER
#if(_MSC_VER<=1200)
#define for if (false) {} else for
#endif
#endif

#define LZ_META_ASSERT(x) typedef char LZ_META_ASSERT_##__LINE__[ (x) ? 1 : -1 ];

typedef unsigned char LZuchar;
typedef unsigned short LZushort;
typedef unsigned int LZuint;

typedef PNGNAME(Image) LightPng_Image;

LZ_META_ASSERT( sizeof( LZuchar ) == 1 );
LZ_META_ASSERT( sizeof( LZushort ) == 2 );
LZ_META_ASSERT( sizeof( LZuint ) == 4 );

#define CRC_START 0xffffffff

#define for if (false) {} else for

// The PNG chunk id's
enum LightPng_ChunkIds
{
	// Mandatory chunks
	Chunk_IHDR = 0x49484452,
	Chunk_PLTE = 0x504C5445,
	Chunk_IDAT = 0x49444154,
	Chunk_IEND = 0x49454E44,

	// Optional chunks
	Chunk_pHYs = 0x70485973,
	Chunk_gAMA = 0x67414D41,
	Chunk_cHRM = 0x6348524D,
	Chunk_tEXt = 0x74455874,
	Chunk_tRNS = 0x74524E53,
};

// The adaptive filter
enum LightPng_Filters
{
	Filter_None,
	Filter_Sub,
	Filter_Up,
	Filter_Average,
	Filter_Paeth,
};

// Class that holds the IDAT chunk data until it's used.
// There is little logic to this. If only one data chunk is added, then it is used in-place.
// If more are added, then new memory is allocated and it's used to store the concatenated result.
class LightPng_Data
{
public:
	const LZuchar *data;
	int data_size;
	bool free_data;				// Is custom allocated?

	LightPng_Data()
		: data( 0 )
		, data_size( 0 )
		, free_data( false )
	{
	}

	~LightPng_Data()
	{
		Reset();
	}

	// Resets the container
	void Reset()
	{
		if ( free_data && data != 0 )
			delete [] (LZuchar *)data;
		data = 0;
		data_size = 0;
		free_data = false;
	}

	// Adds the given data to the data container
	void AddData( const LZuchar *src, int src_size )
	{
		if ( src == 0 || src_size <= 0 )
			return;

		// First data block?
		if ( data == 0 )
		{
			data = src;
			data_size = src_size;
			free_data = false;
			return;
		}

		// Allocate a new buffer
		int new_size = data_size + src_size;
		LZuchar *new_data = new LZuchar[ new_size ];
		if ( new_data == 0 )
			return;

		// Copy the data (old & new)
		for ( int i = 0; i < data_size; ++i )
			new_data[ i ] = data[ i ];
		for ( int i = 0; i < src_size; ++i )
			new_data[ data_size + i ] = src[ i ];

		// Replace buffers
		Reset();
		data = new_data;
		data_size = new_size;
		free_data = true;
	}
};

// The PNG load state
struct LightPng_State
{
	// The source data
	const LZuchar *src;
	int src_left;

	// The error flag
	bool has_errors;

	// The CRC table
	LZuint crc_table[ 256 ];
	// The current CRC
	LZuint crc;

	LightPng_State()
		: src( 0 )
		, src_left( 0 )
		, has_errors( false )
		, crc( CRC_START )
	{
		// Calculate the CRC table
		for ( int n = 0; n < 256; ++n )
		{
			LZuint crc = n;
			for ( int k = 0; k < 8; ++k )
			{
				if ( crc & 1 )
					crc = 0xedb88320 ^ (crc >> 1);
				else
					crc >>= 1;
			}
			crc_table[ n ] = crc;
		}
	}

	// Reads one byte of input
	LZuchar Read()
	{
		if ( src_left <= 0 )
		{
			has_errors = true;
			return 0;
		}

		crc = UpdateCrc( crc, *src );

		--src_left;
		return *src++;
	}

	// Reads an integer (4 bytes) of input
	LZuint ReadInt()
	{
		LZuint ret = Read() << 24;
		ret |= Read() << 16;
		ret |= Read() << 8;
		return ret | Read();
	}

	// Skips x bytes of input
	void Skip( int amt )
	{
		while (amt-- > 0 )
			Read();
	}

	// Updates the CRC by the given data value
	LZuint UpdateCrc( LZuint crc, LZuchar data )
	{
		return crc_table[ (crc ^ data) & 0xff ] ^ (crc >> 8);
	}
};

// The image guardian (frees the image on error/abrupt exit)
class LightPng_Image_Guardian
{
public:
	LightPng_Image *img;

	LightPng_Image_Guardian(): img( new LightPng_Image() ) { }
	~LightPng_Image_Guardian() { if ( img != 0 ) delete img; }

	LightPng_Image *operator -> () { return img; }
};

// Absolute function
static inline int LightPng_Abs( int v )
{
	return (v < 0) ? -v : v;
}

// Pow function
static inline int LightPng_Pow( int base, int exp)
{
	int v = 1;
	while ( exp-- > 0 )
		v *= base;
	return v;
}

// Paeth predictor
static inline LZuchar PaethPredictor( int a, int b, int c )
{
	int p = a + b - c;
	int pa = LightPng_Abs( p - a );
	int pb = LightPng_Abs( p - b );
	int pc = LightPng_Abs( p - c );

	if ( pa <= pb && pa <= pc )
		return (LZuchar) a;
	if ( pb <= pc )
		return (LZuchar) b;
	return (LZuchar) c;
}

PNGNAME(Image)::PNGNAME(Image)()
	: width( 0 )
	, height( 0 )
	, has_palette( false )
	, palette( 0 )
	, palette_size( 0 )
	, data( 0 )
{
}

PNGNAME(Image)::~PNGNAME(Image)()
{
	if ( palette != 0 )
		delete [] palette;
	if ( data != 0 )
		delete [] data;
}

// Create a .png image from the given data.
// Returns null on error.
PNGNAME(Image) *PNGNAME(Create)( const void *data, int data_size, LightZ_Env *z_env )
{
	if ( data == 0 || data_size <= 0 )
		return 0;

	// Create the load state
	LightPng_State state;
	state.src = (const LZuchar *) data;
	state.src_left = data_size;

	LightPng_Image_Guardian img;

	// Read the signature
#define SIG(x) if ( state.Read() != x ) return 0;
	SIG( 137 )
	SIG( 80 )
	SIG( 78 )
	SIG( 71 )
	SIG( 13 )
	SIG( 10 )
	SIG( 26 )
	SIG( 10 )
#undef SIG

	// Read the chunks
	bool need_ihdr = true;

	int color;
	int bitspp;
	int filtered_size = -1;

	// Image data (raw, compressed)
	LightPng_Data compressed_data;

	bool end_found = false;
	while ( !end_found )
	{
		// Read the chunk header
		int chunk_len = state.ReadInt();
		state.crc = CRC_START;
		int chunk_type = state.ReadInt();

		if ( chunk_len > state.src_left )
			return 0;
		if ( state.has_errors )
			return 0;

//		printf( "Chunk type 0x%X: %c%c%c%c\n", chunk_type, (char)((chunk_type>>24)&0xff), (char)((chunk_type>>16)&0xff), (char)((chunk_type>>8)&0xff), (char)((chunk_type>>0)&0xff) );

		// Need a specific chunk?
		if ( need_ihdr )
		{
			if ( chunk_type != Chunk_IHDR )
				return 0;
			need_ihdr = false;
		}

		// Process the chunk
		switch ( chunk_type )
		{
		case Chunk_IHDR: // Header
			{
				img->width = state.ReadInt();
				img->height = state.ReadInt();
				int bpp = state.Read();
				color = state.Read();
				int compression = state.Read();
				int filter = state.Read();
				int ilace = state.Read();

				// Only 24/32bit and palettized images allowed
				switch ( color )
				{
				case 2: // RGB
					bitspp = 24;
					break;

				case 3: // Palettized
				{
					bitspp = bpp;
					img->palette_size = LightPng_Pow( 2, bitspp );
					img->has_palette = true;
					img->palette = new LightPng_Image::Color[ img->palette_size ];
					if ( img->palette == 0 )
						return 0;
					if ( bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8 )
						return 0;
					for ( int i = 0; i < img->palette_size; ++i )
						img->palette[ i ] = 0x00000000;
					}
				    break;

				case 6: // ARGB
					bitspp = 32;
					break;

				default:
					return 0;
				}

				// Only inflate/deflate compression is supported
				if ( compression != 0 )
					return 0;

				// Adaptive filtering required
				if ( filter != 0 )
					return 0;

				// Interlace is not supported
				if ( ilace != 0 )
					return 0;

				// Calculate the filtered image size
				filtered_size = (int)( 1 + (bitspp * img->width + 7) / 8 ) * img->height;
			}
			break;

		case Chunk_PLTE: // Palette
			{
				if ( chunk_len <= 0 || (chunk_len % 3) != 0 )
					return 0;
				if ( (chunk_len / 3) > img->palette_size )
					return 0;
				if ( img->palette == 0 )
					return 0;

				int idx = 0;
				while ( chunk_len > 0 )
				{
					int col = 0xff000000 | (state.Read() << 16);
					col |= state.Read() << 8;
					col |= state.Read();

					img->palette[ idx++ ] = col;
					chunk_len -= 3;
				}
			}
			break;

		case Chunk_IDAT:
			{
				// Collect the IDAT compressed raw image data for later processing
				compressed_data.AddData( state.src, chunk_len );
				state.Skip( chunk_len );
			}
			break;

		case Chunk_IEND: // End chunk
			end_found = true;
			break;

		case Chunk_tRNS: // Transparency
			if ( color != 3 )
			{ // Supported only for palettized images
				state.Skip( chunk_len );
				break;
			}
			if ( chunk_len > 256 )
				return 0;
			if ( img->palette == 0 )
				return 0;

			for ( int i = 0; i < chunk_len; ++i )
			{
				LZuchar a = state.Read();
				img->palette[ i ] = (img->palette[ i ] & 0x00ffffff) | (a << 24);
			}
			break;

		default: // Unknown chunk
			state.Skip( chunk_len );
			break;
		}

		// Test the crc
		int my_crc = state.crc ^ 0xffffffff;
		int test_crc = state.ReadInt();
		if ( my_crc != test_crc )
			return 0;
	}

	/////////////////////////////////////////////

	// Process the IDAT chunk(s). The image data comes from here.
	if ( compressed_data.data_size <= 0 )
		return 0;

	{
		int data8bits_size = 0;

		// Allocate enough memory
		int line_pitch = (int)(1 + (bitspp * img->width + 7) / 8);

		int data_size = line_pitch;
		if ( color == 2 )
		{ // RGB; allocate room for ARGB
			data_size += img->width;
		}

		data_size *= img->height;
		if ( bitspp >= 8)
			img->data = new LZuchar[ data_size ];
		else
		{
			data8bits_size = (1 * img->width);
			data8bits_size *= img->height;
			img->data = new LZuchar[ data8bits_size ];
		}
		if ( img->data == 0 )
			return 0;

		// Unpack the image
		const char *err = ZFN(Inflate)( compressed_data.data, compressed_data.data_size, &img->data, &data_size, z_env );
		if ( err != 0 )
			return 0;
		if ( data_size != filtered_size )
			return 0;

		// Unfilter the images
		for ( int y = 0; y < img->height; ++y )
		{
			LZuchar *pos = &img->data[ line_pitch * y ];
			LZuchar filter = *pos++;
			if ( filter == Filter_None )
				continue;
			else if (bitspp < 8) //less than 8 bits not supported
				return 0;

			for ( int x = 0; x < img->width; ++x )
			{
				for ( int byte = 0; byte < (int)((bitspp+7) / 8); ++byte, ++pos )
				{
					LZuchar val = *pos;
					LZuchar prev = (x == 0) ? 0 : *(pos-(int)((bitspp+7) / 8));
					LZuchar prior = (y == 0) ? 0 : *(pos-line_pitch);
					LZuchar prior_prev = (x == 0 || y == 0) ? 0 : *(pos-(int)((bitspp+7) / 8)-line_pitch);

					switch ( filter )
					{
					case Filter_Sub:
						*pos = (val + prev) & 0xff;
						break;

					case Filter_Up:
						*pos = (val + prior) & 0xff;
						break;

					case Filter_Average:
						*pos = (val + ((prev+prior)>>1)) & 0xff;
						break;

					case Filter_Paeth:
						*pos = (val + PaethPredictor( prev, prior, prior_prev )) & 0xff;
						break;

					default: // Unknown filter
						return 0;
					}
				}
			}
		}

		// Remove the filter bytes from each scanline
		for ( int y = 0; y < img->height; ++y )
		{
			LZuchar *dest = &img->data[ (line_pitch-1) * y ];
			LZuchar *src = &img->data[ 1 + (line_pitch * y) ];

			int left = line_pitch - 1;
			while ( left-- > 0 )
				*dest++ = *src++;
		}
		--line_pitch; // no more filter bytes

		// Make the buffer 1,2,4bits ->to-> 8bits
		if (color == 3 && data8bits_size != 0)
		{
			int new_line_pitch = img->width;

			for ( int y = img->height - 1; y >= 0; --y )
			{
				LZuchar *s = &img->data[ line_pitch * y ];
				LZuchar *d = &img->data[ new_line_pitch * y ];

				int divider = (8 / bitspp);
				int skipBits = 0;
				if (img->width % divider)
					skipBits = 1;
				for ( int x = (img->width / divider) - 1; x >= 0; --x )
				{
					if (bitspp == 4)
					{
						if (skipBits)
						{
							d[ x * divider + 2 ] = (s[ x + 1 ] >> 4) & 0xf;
							skipBits = 0;
						}
						d[ x * divider + 1 ] = (s[ x ]) & 0xf;
						d[ x * divider ] = (s[ x ] >> 4) & 0xf;
					}
					else if (bitspp == 2)
					{
						if (skipBits)
						{
							int shift = 0;
							for (int i = 7; i >= 4; --i, shift +=2)
							{
								if (x * divider + i < img->width)
									d[ x * divider + i ] = (s[ x + 1 ] >> shift) & 0x3;
							}
							skipBits = 0;
						}
						d[ x * divider + 3 ] = (s[ x ]) & 0x3;
						d[ x * divider + 2 ] = (s[ x ] >> 2) & 0x3;
						d[ x * divider + 1 ] = (s[ x ] >> 4) & 0x3;
						d[ x * divider ] = (s[ x ] >> 6) & 0x3;
					}
					else if (bitspp == 1)
					{
						if (skipBits)
						{
							int shift = 0;
							for (int i = 15; i >= 8; --i, shift ++)
							{
								if (x * divider + i < img->width)
									d[ x * divider + i ] = (s[ x + 1 ] >> shift) & 0x1;
							}
							skipBits = 0;
						}
						d[ x * divider + 7 ] = (s[ x ]) & 0x1;
						d[ x * divider + 6 ] = (s[ x ] >> 1) & 0x1;
						d[ x * divider + 5 ] = (s[ x ] >> 2) & 0x1;
						d[ x * divider + 4 ] = (s[ x ] >> 3) & 0x1;
						d[ x * divider + 3 ] = (s[ x ] >> 4) & 0x1;
						d[ x * divider + 2 ] = (s[ x ] >> 5) & 0x1;
						d[ x * divider + 1 ] = (s[ x ] >> 6) & 0x1;
						d[ x * divider ] = (s[ x ] >> 7) & 0x1;
					}
				}
			}
			line_pitch = new_line_pitch;
		}

		// Extend the RGB -> RGBA (copy backwards)
		if ( color == 2 )
		{
			int new_line_pitch = img->width * 4;

			for ( int y = img->height - 1; y >= 0; --y )
			{
				LZuchar *s = &img->data[ line_pitch * y ];
				LZuchar *d = &img->data[ new_line_pitch * y ];

				for ( int x = img->width - 1; x >= 0; --x )
				{
					int x1 = x * 3;
					int x2 = x * 4;
					d[ x2 + 3 ] = 0xff;
					d[ x2 + 2 ] = s[ x1 + 2 ];
					d[ x2 + 1 ] = s[ x1 + 1 ];
					d[ x2 + 0 ] = s[ x1 + 0 ];
				}
			}

			line_pitch = new_line_pitch;
			color = 6;
		}

		// Convert RGBA -> ARGB
		if ( color == 2 || color == 6 )
		{
			for ( int y = 0; y < img->height; ++y )
			{
				LZuchar *pos = &img->data[ line_pitch * y ];
				int left = img->width;
				while ( left-- > 0 )
				{
					LZuchar r = *(pos + 0);
					LZuchar g = *(pos + 1);
					LZuchar b = *(pos + 2);
					LZuchar a = *(pos + 3);
					*pos++ = a;
					*pos++ = r;
					*pos++ = g;
					*pos++ = b;
				}
			}
		}
	}

	// Release the image and return it
	LightPng_Image *ret = img.img;
	img.img = 0;
	return ret;
}
