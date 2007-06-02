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
#define LIGHTZ_INTERNAL
#include "LightZ.h"
//#include <stdio.h>


#define LZ_META_ASSERT(x) typedef char LZ_META_ASSERT_##__LINE__[ (x) ? 1 : -1 ];

typedef unsigned char LZuchar;
typedef unsigned short LZushort;
typedef unsigned int LZuint;

LZ_META_ASSERT( sizeof( LZuchar ) == 1 );
LZ_META_ASSERT( sizeof( LZushort ) == 2 );
LZ_META_ASSERT( sizeof( LZuint ) == 4 );

#define ADLER32_BASE 65521
#define ADLER32_START 1
#define MAX_BITS 15

#define HUFFSIZE_LEN 2048
#define HUFFSIZE_DIST 1024
#define HUFFSIZE_CODES 512

#define MAX_HUFFMAN_VALUES 300

#define for if (false) {} else for

// The error message to return
#define LZ_ERRORMSG(x) x
//#define LZ_ERRORMSG(x) "LightZ error!"

// Length base values (codes 257-287)
static const int g_len_base[] =
{
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13,
	15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
	67, 83, 99, 115, 131, 163, 195, 227, 258,
	0, 0
};
// Extra bits for lengths (codes 257-287)
static const int g_len_extra[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
	4, 4, 4, 4, 5, 5, 5, 5, 0,
	0, 0
};
// Distance base values (0-31)
static const int g_dist_base[] =
{
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25,
	33, 49, 65, 97, 129, 193, 257, 385, 513, 769,
	1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577,
	0, 0
};
// Extra bits for distance (0-31)
static const int g_dist_extra[] =
{
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3,
	4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
	9, 9, 10, 10, 11, 11, 12, 12, 13,13,
	0, 0
};
// Code length write order
static const int g_codelen_order[] =
{
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

// The adler32 update
static inline LZuint UpdateAdler32( LZuint adler, LZuchar *buf, int len )
{
	LZuint s1 = adler & 0xffff;
	LZuint s2 = (adler >> 16) & 0xffff;

	for ( int n = 0; n < len; ++n )
	{
		s1 = (s1 + buf[ n ]) % ADLER32_BASE;
		s2 = (s2 + s1)       % ADLER32_BASE;
	}

	return (s2 << 16) + s1;
}

static inline LZuint UpdateAdler32( LZuint adler, LZuchar chr )
{
	LZuint s1 = adler & 0xffff;
	LZuint s2 = (adler >> 16) & 0xffff;

	s1 = (s1 + chr) % ADLER32_BASE;
	s2 = (s2 + s1)  % ADLER32_BASE;

	return (s2 << 16) + s1;
}

// One Huffman tree node
struct LightZ_HuffNode
{
	// The table index to jump to if the next bit is 1. If 0, then this is a literal node. If 0xffff, then this is an error node.
	// If the next bit is 0, then the next table index is (this+1).
	LZushort jump_if_1;

	// If "jump_if_1" is 0, then this is the literal value for this node.
	LZushort value;
};

// One Huffman value/code pair
struct LightZ_HuffCode
{
	LZushort bits;
	LZushort code;
	LZushort value;
};

///////////////////////////////////
///////////////////////////////////
////////////   DEBUG   ////////////
///////////////////////////////////
///////////////////////////////////

/*
static void ShowBits( int num, int bits )
{
	for ( int i = 0; i < bits; ++i )
	{
		int shift = bits - i - 1;
		int bit = (num >> shift) & 1;
		printf( "%d", bit );
	}
}

static void ShowBitLenList( const char *title, LZuchar *list, int size )
{
	printf( "%s\n", title );
	for ( int i = 0; i < size; ++i )
	{
		printf( "%3d: %d\n", i, list[ i ] );
	}
}

static void ShowHuffTree( LightZ_HuffNode *tree, const char *prefix, int node, int indent )
{
	LightZ_HuffNode *n = &tree[ node ];

	for ( int i = 0; i < indent; ++i )
		printf( " " );

	printf( "%s[%d] ", prefix, node );
	if ( n->jump_if_1 == 0 )
		printf( "Value: %d\n", n->value );
	else if ( n->jump_if_1 == 0xffff )
		printf( "ERROR NODE\n" );
	else
	{
		printf( "\n" );
		ShowHuffTree( tree, "Zero-> ", node + 1, indent + 2 );
		ShowHuffTree( tree, "One -> ", n->jump_if_1, indent + 2 );
	}
}
*/

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

// Sorts a table of Huffman codes (increasing order by code&bits).
static void SortHuffmanCodes( LightZ_HuffCode *codes, int amt )
{
	// TODO: Faster sort code!
	for ( int i = 0; i < amt; ++i )
	{
		for ( int j = i + 1; j < amt; ++j )
		{
			if ( codes[ i ].code <= codes[ j ].code )
				continue;

			LightZ_HuffCode tmp = codes[ i ];
			codes[ i ] = codes[ j ];
			codes[ j ] = tmp;
		}
	}
}

// Builds the Huffman tree nodes from a tree-sorted Huffman code table
static int BuildHuffmanNodes( int cur_bit, int code_idx, LightZ_HuffCode *codes, int code_amt, LightZ_HuffNode *dest_tree, int dest_tree_pos, int dest_tree_max_size )
{
	if ( dest_tree_pos >= dest_tree_max_size )
		return -1;
	if ( code_idx >= code_amt )
		return -1;

	if ( cur_bit == codes[ code_idx ].bits )
	{ // Found an ending leaf node
		dest_tree[ dest_tree_pos ].jump_if_1 = 0;
		dest_tree[ dest_tree_pos ].value = codes[ code_idx ].value;
		return dest_tree_pos + 1;
	}

	// Make a knot node (branch)
	LightZ_HuffNode *knot = &dest_tree[ dest_tree_pos++ ];
	knot->jump_if_1 = 0xffff; // (will be changed later, just a temp value)

	if ( dest_tree_pos >= dest_tree_max_size )
		return -1;

	// Get the next bit (counting from left; MSB side)
	int bit = (codes[ code_idx ].code >> (MAX_BITS-1-cur_bit)) & 1;

	if ( bit == 0 )
	{ // Add the bit 0 -subtree
		dest_tree_pos = BuildHuffmanNodes( cur_bit + 1, code_idx, codes, code_amt, dest_tree, dest_tree_pos, dest_tree_max_size );
		if ( dest_tree_pos < 0 )
			return -1;
	}
	else
	{ // Cannot continue with a zero bit
		dest_tree[ dest_tree_pos++ ].jump_if_1 = 0xffff;
		if ( dest_tree_pos >= dest_tree_max_size )
			return -1;
	}

	knot->jump_if_1 = dest_tree_pos;

	// Scroll to a position where this bit is one (start of the other subtree)
	while ( code_idx < code_amt )
	{
		bit = (codes[ code_idx ].code >> (MAX_BITS-1-cur_bit)) & 1;
		if ( bit == 1 )
			break;
		++code_idx;
	}

	// Bit 1 not found?
	if ( code_idx >= code_amt )
	{
		dest_tree[ dest_tree_pos++ ].jump_if_1 = 0xffff;
		return dest_tree_pos;
	}

	// Add the bit 1 subtree
	dest_tree_pos = BuildHuffmanNodes( cur_bit + 1, code_idx, codes, code_amt, dest_tree, dest_tree_pos, dest_tree_max_size );
	return dest_tree_pos;
}

// Builds a huffman tree.
static bool BuildHuffmanTree( LZuchar *val_bits, int values, LightZ_HuffNode *dest_tree, int dest_tree_max_size, LightZ_HuffCode *temp_codes )
{
	// Count how many values there are for each bit length
	int values_per_bitlen[ MAX_BITS + 1 ];

	for ( int i = 0; i <= MAX_BITS; ++i )
		values_per_bitlen[ i ] = 0;
	for ( int i = 0; i < values; ++i )
	{
		int bits = val_bits[ i ];
		if ( bits < 0 || bits > MAX_BITS )
			return false;
		values_per_bitlen[ bits ]++;
	}
	values_per_bitlen[ 0 ] = 0;

	// Test for too few/too many bits allocated
	if ( values > 1 )
	{
		int left = 1;
		for ( int i = 1; i <= MAX_BITS; ++i )
		{
			left <<= 1;
			left -= values_per_bitlen[ i ];
			// Too many bits?
			if ( left < 0 )
				return false;
		}
		// Too few bits?
		if ( left > 0 )
			return false;
	}
	else
	{ // Either 0 or 1 values
		if ( values_per_bitlen[ 1 ] != values )
			return false;
	}

	// Find a numerical smallest value for each of the bit lengths
	int next_code[ MAX_BITS + 1 ];
	int code = 0;
	for ( int bits = 1; bits <= MAX_BITS; ++bits )
	{
		code = ( code + values_per_bitlen[ bits - 1 ] ) << 1;
		next_code[ bits ] = code;
	}

	// Generate the code values
	if ( values > MAX_HUFFMAN_VALUES )
		return false;
	LightZ_HuffCode *codes = temp_codes;
	int code_amt = 0;

//	printf( "Huffman codes:\n" );
	for ( int i = 0; i < values; ++i )
	{
		int bits = val_bits[ i ];
		if ( bits == 0 )
			continue;

		codes[ code_amt ].bits = bits;
		codes[ code_amt ].code = next_code[ bits ] << (MAX_BITS - bits); // Make it so that the first bit is always at the left side
		codes[ code_amt ].value = (LZushort) i;

//		printf( "Val: %d\tBits: %d\tCode: ", i, bits );
//		ShowBits( next_code[ bits ], bits );
//		printf( "\n" );

		++code_amt;
		next_code[ bits ]++;
	}

	// Sort the codes to increasing order
	SortHuffmanCodes( codes, code_amt );

	// Init the tree construction
	dest_tree[ 0 ].jump_if_1 = 0xffff;

	bool ok = true;
	if ( code_amt > 0 )
	{
		if ( BuildHuffmanNodes( 0, 0, codes, code_amt, dest_tree, 0, dest_tree_max_size ) < 0 )
			ok = false;
	}

//	if ( ok ) ShowHuffTree( dest_tree, "", 0, 0 );

	return ok;
}

// The memory allocated environment
struct LightZ_Env
{
	// Allocations for prebuilt length/distance trees
	LightZ_HuffNode pre_len[ HUFFSIZE_LEN ];
	LightZ_HuffNode pre_dist[ HUFFSIZE_DIST ];

	// Allocations for dynamic length/distance trees
	LightZ_HuffNode dyn_len[ HUFFSIZE_LEN ];
	LightZ_HuffNode dyn_dist[ HUFFSIZE_DIST ];

	// Allocations for dynamic code length trees
	LightZ_HuffNode codelen[ HUFFSIZE_CODES ];

	// Allocation for bit list big enough for lengths & distances
	LZuchar temp_bits_list[ 512 ];

	// Temporary for huffman code values (used during tree construction)
	LightZ_HuffCode huffman_values[ MAX_HUFFMAN_VALUES ];


	LightZ_Env()
	{
		// Build the predefined length list
		for ( int i = 0; i <= 143; ++i )
			temp_bits_list[ i ] = 8;
		for ( int i = 144; i <= 255; ++i )
			temp_bits_list[ i ] = 9;
		for ( int i = 256; i <= 279; ++i )
			temp_bits_list[ i ] = 7;
		for ( int i = 280; i <= 287; ++i )
			temp_bits_list[ i ] = 8;

		BuildHuffmanTree( temp_bits_list, 288, pre_len, HUFFSIZE_LEN, huffman_values );

		// Build the predefined distance list
		for ( int i = 0; i <= 31; ++i )
			temp_bits_list[ i ] = 5;

		BuildHuffmanTree( temp_bits_list, 32, pre_dist, HUFFSIZE_DIST, huffman_values );
	}

	~LightZ_Env()
	{
	}
};

// Scope guard for the environment
class LightZ_Env_Guard
{
public:
	LightZ_Env *ptr;

	LightZ_Env_Guard() : ptr( 0 ) { }
	~LightZ_Env_Guard() { if ( ptr != 0 ) delete ptr; }
};

// The unpacking state
struct LightZ_State
{
	// The memory environment
	LightZ_Env *env;

	// The source data
	const LZuchar *src;
	int src_left;

	bool src_reallocated; // Flag to see if src has been reallocated (only in the case of overlapping areas)
	LZuchar *release_src; // In case src has been reallocated, this is the pointer that should be released

	// The destination data
	LZuint dest_adler32;
	LZuchar **dest;
	int *dest_pos;
	int dest_size;

	bool src_and_dest_overlap; // If source and destination areas 

	// Currently being read byte (for bit reading)
	int read_byte;
	int read_byte_left;

	// The sliding window to backwards (written) data
	int window_size;

	// Current error message (null == all ok)
	const char *err_msg;



	LightZ_State()
		: src( 0 )
		, src_left( 0 )
		, src_reallocated( 0 )
		, release_src( 0 )
		, dest_adler32( ADLER32_START )
		, dest( 0 )
		, dest_pos( 0 )
		, dest_size( 0 )
		, src_and_dest_overlap( false )
		, read_byte( 0 )
		, read_byte_left( 0 )
		, window_size( 0 )
		, err_msg( 0 )
	{ }

	~LightZ_State()
	{
		if ( release_src != 0 )
			delete [] release_src;
		release_src = 0;
	}

	// Reads one character of input.
	LZuchar Read()
	{
		if ( src == 0 || src_left <= 0 )
		{
			if ( err_msg == 0 )
				err_msg = LZ_ERRORMSG("Out of source data (EOS)!");
			return 0;
		}

		--src_left;
		return *src++;
	}

	// Reads x bits of data
	int ReadBits( int bits )
	{
		int ret = 0;

		int shift = 0;
		while ( bits > 0 )
		{
			if ( read_byte_left == 0 )
			{
				read_byte = Read();
				read_byte_left = 8;
			}

			// Grab the bits
			int amt = (bits < read_byte_left) ? bits : read_byte_left;
			int new_bits = read_byte & ( (1<<amt) - 1 );
			ret |= new_bits << shift;

			// Advance
			bits -= amt;
			shift += amt;
			read_byte >>= amt;
			read_byte_left -= amt;
		}

		return ret;
	}

	// Reads a Huffman packed value, given the Huffman decoding tree (-1 on error)
	int ReadHuffman( LightZ_HuffNode *tree )
	{
		if ( tree == 0 )
			return -1;

		int idx = 0;
		while ( err_msg == 0 )
		{
			int jump_if_1 = tree[ idx ].jump_if_1;

			if ( jump_if_1 == 0 )
				return tree[ idx ].value;
			if ( jump_if_1 == 0xffff )
				return -1;

			int bit = ReadBits( 1 );

			if ( bit == 0 )
				++idx;
			else
				idx = jump_if_1;
		}

		return -1;
	}

	// Writes one character of data
	void Write( LZuchar chr )
	{
		if ( !EnsureFreeSize( 1 ) )
			return;

		// Check for dest and source overlaps
		if ( src_and_dest_overlap )
			CheckForDestOverlap( 1 );

		// Write the data
		dest_adler32 = UpdateAdler32( dest_adler32, chr );
		(*dest)[ (*dest_pos)++ ] = chr;
	}

	// Writes data from back window
	void WriteBack( int dist, int len )
	{
		int start_pos = *dest_pos - dist;

		if ( dist <= 0 || len <= 0 )
			return;
		if ( start_pos < 0 )
		{
			if ( err_msg == 0 )
				err_msg = LZ_ERRORMSG("Unable to refer outside the unpacked data boundary!");
			return;
		}

		if ( !EnsureFreeSize( len ) )
			return;

		// Check for dest and source overlaps
		if ( src_and_dest_overlap )
			CheckForDestOverlap( len );

		// Copy the data
		int left = len;
		LZuchar *s = &((*dest)[ start_pos ]);
		LZuchar *d = &((*dest)[ *dest_pos ]);

		while ( left-- > 0 )
			*d++ = *s++;
		*dest_pos += len;

		// Update the checksum
		dest_adler32 = UpdateAdler32( dest_adler32, &((*dest)[ start_pos ]), len );
	}

private:
	// Preallocate a small buffer for overlapping src copying
	enum { STATIC_SRC_BUFFER_SIZE = 16 };
	LZuchar m_src_prealloc_buffer[ STATIC_SRC_BUFFER_SIZE ];


	// Ensures free destination size for at least x more bytes
	// Returns false in case of an error.
	bool EnsureFreeSize( int free )
	{
		if ( free <= 0 )
			return true;

		int cur_free = dest_size - *dest_pos;
		if ( cur_free >= free )
			return true;

		// If source and destination buffers overlap, then dest cannot be freed/reallocated.
		if ( src_and_dest_overlap )
		{
			const char *tmp = LZ_ERRORMSG("Unable to resize the destination buffer when it overlaps with the source data memory.");
			err_msg = tmp;
			return false;
		}

		// Get the amount to allocate
		int alloc_more = 16 + (free - cur_free) + src_left + (src_left >> 3);
		int new_size = dest_size + alloc_more;

		// Allocate the new buffer
		LZuchar *dold = *dest;
		LZuchar *dnew = new LZuchar[ new_size ];

		if ( dnew == 0 )
		{
			err_msg = LZ_ERRORMSG("Out of memory error!");
			return false;
		}

		// Copy old data
		if ( dold != 0 )
		{
			LZuchar *s = dold;
			LZuchar *d = dnew;
			int left = *dest_pos;
			while ( left-- > 0 )
				*d++ = *s++;
		}

		// Switch to new buffer
		*dest = dnew;
		dest_size = new_size;

		if ( dold != 0 )
			delete [] dold;

		return true;
	}

	// Called when we know that the source and destination areas overlap. This function checks if
	// the overlap actually occurs now.
	void CheckForDestOverlap( int add_amt )
	{
		if ( src_reallocated || src_left <= 0 || add_amt <= 0 )
			return;

		// Would this write make dest catch up to source?
		LZuchar *d_after = *dest + (*dest_pos + add_amt);
		if ( d_after < src )
			return;

		// Allocate a new source buffer for the rest of the data
		bool use_prealloc = ( src_left <= STATIC_SRC_BUFFER_SIZE );
		LZuchar *new_src = (use_prealloc ? m_src_prealloc_buffer : new LZuchar[ src_left ]);

		if ( new_src == 0 )
		{
			if ( err_msg == 0 )
			{
				const char *tmp = LZ_ERRORMSG( "Error allocating a new source buffer (destination and source buffers overlap)." );
				err_msg = tmp;
			}
			return;
		}

		// Copy the remaining data
		int left = src_left;
		const LZuchar *s = src;
		LZuchar *d = new_src;

		while ( left-- > 0 )
			*d++ = *s++;

		// Switch to new source
		src = new_src;
		src_reallocated = true;
		if ( !use_prealloc )
			release_src = new_src;
	}
};

// Unpacks the bit lengths by using the code length Huffman tree
static bool UnpackCodeLens( LightZ_State &state, int read_amt, LZuchar *dest, int dest_size )
{
	LightZ_HuffNode *huff_codelen = state.env->codelen;

	for ( int i = 0; i < dest_size; ++i )
		dest[ i ] = 0;

	int prev_code = -1;
	while ( read_amt > 0 )
	{
		int code = state.ReadHuffman( huff_codelen );
		if ( code < 0 )
		{
			if ( state.err_msg == 0 )
				state.err_msg = LZ_ERRORMSG("Invalid Huffman code in code lengths!");
			return false;
		}

		int amt = 1;
		switch ( code )
		{
		case 16:	amt = 3 + state.ReadBits( 2 ); code = prev_code; break;
		case 17:	amt = 3 + state.ReadBits( 3 ); code = 0; break;
		case 18:	amt = 11 + state.ReadBits( 7 ); code = 0; break;
		}

		if ( code < 0 || code > 15 )
		{
			if ( state.err_msg == 0 )
				state.err_msg = LZ_ERRORMSG("Invalid code to repeat (in dynamic Huffman construction)!");
			return false;
		}
		if ( amt > read_amt )
		{
			if ( state.err_msg == 0 )
				state.err_msg = LZ_ERRORMSG("Too many codes (in dynamic Huffman construction)!");
			return false;
		}

		read_amt -= amt;
		while ( amt-- > 0 )
			*dest++ = (LZuchar) code;

		prev_code = code;
	}

	return true;
}

// Unpacks the dynamic Huffman tables from the input
static void UnpackDynamicHuffman( LightZ_State &state )
{
	LightZ_Env *env = state.env;
	LZuchar *temp_bits_list = env->temp_bits_list;

	int lengths = 257 + state.ReadBits( 5 ); // HLIT
	int dists = 1 + state.ReadBits( 5 ); // HDIST
	int codelens = 4 + state.ReadBits( 4 ); // HCLEN

	// Read the code lengths
	for ( int i = 0; i < 19; ++i )
		temp_bits_list[ i ] = 0;
	for ( int i = 0; i < codelens; ++i )
		temp_bits_list[ g_codelen_order[ i ] ] = state.ReadBits( 3 );

//	ShowBitLenList( "Code lengths (in bits):", temp_bits_list, 19 );

	// Build the codelen Huffman tree
	if ( !BuildHuffmanTree( temp_bits_list, 19, env->codelen, HUFFSIZE_CODES, env->huffman_values ) )
	{
		if ( state.err_msg == 0 )
			state.err_msg = LZ_ERRORMSG("Unable to build the Huffman tree for code lengths.");
		return;
	}

	// Read the dynamic lengths and distances
	if ( !UnpackCodeLens( state, lengths + dists, temp_bits_list, lengths + dists ) )
		return;

	// Build the Huffman tree (dynamic lengths)
	if ( !BuildHuffmanTree( temp_bits_list, lengths, env->dyn_len, HUFFSIZE_LEN, env->huffman_values ) )
	{
		if ( state.err_msg == 0 )
			state.err_msg = LZ_ERRORMSG("Unable to build the dynamic Huffman tree for lengths.");
		return;
	}

	// Build the Huffman tree (dynamic distances)
	if ( !BuildHuffmanTree( &temp_bits_list[ lengths ], dists, env->dyn_dist, HUFFSIZE_DIST, env->huffman_values ) )
	{
		if ( state.err_msg == 0 )
			state.err_msg = LZ_ERRORMSG("Unable to build the dynamic Huffman tree for distances.");
		return;
	}
}

// Inflates one block of data. Return false if this is the last block.
static bool InflateBlock( LightZ_State &state )
{
	bool BFINAL = (state.ReadBits( 1 ) == 0) ? false : true;
	int BTYPE = state.ReadBits( 2 );

	if ( BTYPE == 0 )
	{ // No compression
		// Skip partial bits
		state.read_byte_left = 0;

		// Read LEN and NLEN
		int LEN = state.Read();
		LEN |= state.Read() << 8;
		int NLEN = state.Read();
		NLEN |= state.Read() << 8;

		if ( LEN != ((~NLEN) & 0xffff) )
		{
			if ( state.err_msg == 0 )
				state.err_msg = LZ_ERRORMSG("Invalid LEN/NLEN pair in uncompressed data!");
			return false;
		}

		// Copy LEN bytes to output
		int left = LEN;
		while ( left-- > 0 )
		{
			LZuchar v = state.Read();
			state.Write( v );
		}
	}
	else if ( BTYPE != 3 )
	{ // Fixed(1)/Dynamic(2) Huffman codes
		LightZ_HuffNode *huff_len = state.env->pre_len;
		LightZ_HuffNode *huff_dist = state.env->pre_dist;

		// Do we have a dynamic Huffman table?
		if ( BTYPE == 2 )
		{
			UnpackDynamicHuffman( state );

			huff_len = state.env->dyn_len;
			huff_dist = state.env->dyn_dist;
		}

		while ( state.err_msg == 0 )
		{
			// Read the length (or literal)
			int len = state.ReadHuffman( huff_len );
			if ( len == -1 )
			{
				if ( state.err_msg == 0 )
					state.err_msg = LZ_ERRORMSG("Invalid Huffman length code!");
				return false;
			}

			// Process the code
			if ( len <= 0xff )
			{ // Literal
				state.Write( (LZuchar) len );
			}
			else if ( len == 256 )
			{ // End of block
				break;
			}
			else
			{ // Length
				// Get the real length
				len = g_len_base[ len - 257 ] + state.ReadBits( g_len_extra[ len - 257 ] );

				// Get the distance
				int dist = state.ReadHuffman( huff_dist );
				if ( dist < 0 )
				{
					if ( state.err_msg == 0 )
						state.err_msg = LZ_ERRORMSG("Invalid Huffman distance code!");
					return false;
				}
				dist = g_dist_base[ dist ] + state.ReadBits( g_dist_extra[ dist ] );

				// Copy len bytes from dist bytes earlier
				state.WriteBack( dist, len );
			}
		}
	}
	else
	{ // Reserved (error)
		if ( state.err_msg == 0 )
			state.err_msg = LZ_ERRORMSG("Invalid compressed block BTYPE!");
	}

	return !BFINAL;
}

// Allocates a new LightZ environment
LightZ_Env *ZFN(NewEnv)()
{
	return new LightZ_Env();
}

// Deallocates a LightZ environment
void ZFN(NewEnv)( LightZ_Env *env )
{
	if ( env != 0 ) delete env;
}

// Inflates the given source data.
// Params:
//  source        - The source data pointer (non-null if source_len > 0)
//  source_len    - Length of the source data
//  dest          - Pointer to the destination data area.
//                  If the destination needs enlargement, the old pointer is delete []:d away.
//  dest_len      - Pointer to the current destination data area size.
//                  The value will be updated to contain the new unpacked data size.
// If everything was ok, return null. Otherwise the return value is the error message.
const char *ZFN(Inflate)(
	const void *source, int source_len,
	unsigned char **dest, int *dest_len,
	LightZ_Env *env )
{
	// Basic checks
	if ( dest == 0 )
	{
		const char *temp = LZ_ERRORMSG("Destination buffer pointer is null!");
		return temp;
	}
	if ( dest_len == 0 )
	{
		const char *temp = LZ_ERRORMSG("Destination buffer length pointer is null!");
		return temp;
	}
	if ( *dest_len < 0 )
	{
		const char *temp = LZ_ERRORMSG("Negative destination buffer lengths are invalid!");
		return temp;
	}

	if ( source_len < 0 )
	{
		const char *temp = LZ_ERRORMSG("Invalid source data length (<0)!");
		return temp;
	}
	if ( source == 0 || source_len == 0 )
	{
		*dest_len = 0;
		if ( source_len > 0 )
		{
			const char *temp = LZ_ERRORMSG("Source length is >0 but the source data pointer is null!");
			return temp;
		}
		return 0;
	}

	if ( source_len < 8 )
	{
		const char *temp = LZ_ERRORMSG("Source length is too small (<8) to hold valid packed data!");
		return temp;
	}

	// Create the environment (if needed)
	LightZ_Env_Guard env_guard;

	if ( env == 0 )
	{
		env = new LightZ_Env();
		env_guard.ptr = env;
	}

	// Init the unpack state
	LightZ_State state;
	state.env = env;

	state.src = (const LZuchar *) source;
	state.src_left = source_len;

	state.dest = dest;
	state.dest_pos = dest_len;
	state.dest_size = *dest_len;

	*state.dest_pos = 0;

	// Check if source and destination overlap
	if ( *dest != 0 )
	{
		const LZuchar *ptr = (const LZuchar *) *dest;
		int len = state.dest_size;
		state.src_and_dest_overlap = !((ptr+len)<=state.src || (state.src+state.src_left)<=ptr);
	}

	// Parse the CMF header
	LZuchar CMF = state.Read();
	LZuchar CM = CMF & 0x0f;
	LZuchar CINFO = (CMF >> 4) & 0x0f;

	if ( CM != 8 )
	{
		const char *temp = LZ_ERRORMSG("Unknown compression method (only deflate/inflate is supported)!");
		return temp;
	}   
	if ( CINFO > 7 )
	{
		const char *temp = LZ_ERRORMSG("Too big LZ77 window size!");
		return temp;
	}   
	
	state.window_size = 1 << (8 + CINFO);

	// Parse the FLG header
	LZuchar FLG = state.Read();
	LZuchar FDICT = (FLG >> 5) & 1;

	// Test the header checksum
	if ( (((CMF<<8) | FLG) % 31) != 0 )
	{
		const char *temp = LZ_ERRORMSG("Header checksum error!");
		return temp;
	}   

	// Uncompress the dictionary (if any)
	int dict_size = 0;
	if ( FDICT != 0 )
	{
		LZuint dict_id = state.Read() << 24;
		dict_id |= state.Read() << 16;
		dict_id |= state.Read() << 8;
		dict_id |= state.Read() << 0;

		// Unpack the dictionary
		while ( state.err_msg == 0 )
		{
			if ( !InflateBlock( state ) )
				break;
		}

		if ( state.err_msg != 0 )
			return state.err_msg;

		if ( dict_id != state.dest_adler32 )
		{ // Invalid checksum
			const char *temp = LZ_ERRORMSG("Invalid dictionary Adler32 checksum!");
			return temp;
		}

		// Reset the dictionary changes
		state.dest_adler32 = ADLER32_START;
		dict_size = *state.dest_pos;
	}

	// Unpack the data
	while ( state.err_msg == 0 )
	{
		if ( !InflateBlock( state ) )
			break;
	}

	if ( state.err_msg != 0 )
		return state.err_msg;

	// Remove the dictionary
	if ( dict_size > 0 )
	{
		LZuchar *mem = *dest;
		LZuchar *d = mem;
		LZuchar *s = &mem[ dict_size ];

		*dest_len -= dict_size;
		int left = *dest_len;

		while ( left-- > 0 )
			*d++ = *s++;
	}

	// Should have at least 4 bytes left (adler32 checksum)
	if ( state.src_left < 4 )
	{
		const char *temp = LZ_ERRORMSG("Out of data error (checksum missing)!");
		return temp;
	}
	LZuint adler32 = state.Read() << 24;
	adler32 |= state.Read() << 16;
	adler32 |= state.Read() << 8;
	adler32 |= state.Read() << 0;

	if ( state.dest_adler32 != adler32 )
	{
		const char * temp = LZ_ERRORMSG("Adler32 checksum error!");
		return temp;
	}

	// Too much data?
//	if ( state.src_left > 0 )
//		return LZ_ERRORMSG("Too much data; some left unused!");
    
	return 0;
}
