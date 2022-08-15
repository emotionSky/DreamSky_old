#ifndef __GET_BITS_H__
#define __GET_BITS_H__

#include "common_type.h"
#include <cstdint>

#ifndef UNCHECKED_BITSTREAM_READER
#define UNCHECKED_BITSTREAM_READER !CONFIG_SAFE_BITSTREAM_READER
#endif

#ifndef CACHED_BITSTREAM_READER
#define CACHED_BITSTREAM_READER 0
#endif

struct GetBitContext 
{
	const uint8_t* buffer, * buffer_end;
#if CACHED_BITSTREAM_READER
	uint64_t cache;
	unsigned bits_left;
#endif
	int index;
	int size_in_bits;
	int size_in_bits_plus8;
};

/**
 * Read 1-25 bits.
 */
DREAMSKY_API unsigned int get_bits(GetBitContext* s, int n);

/**
 * Show 1-25 bits.
 */
DREAMSKY_API void skip_bits(GetBitContext* s, int n);

/**
 * Show 1-25 bits.
 */
DREAMSKY_API unsigned int show_bits(GetBitContext* s, int n);

#endif //!__GET_BITS_H__