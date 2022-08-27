#ifndef __BITS_DEFINE_H__
#define __BITS_DEFINE_H__

#include "common_type.h"
#include "Component/common/common_math.h"
#include "Component/bits_operation/int_read_write.h"

#if CACHED_BITSTREAM_READER
#   define MIN_CACHE_BITS 64
#elif defined LONG_BITSTREAM_READER
#   define MIN_CACHE_BITS 32
#else
#   define MIN_CACHE_BITS 25
#endif

#if !CACHED_BITSTREAM_READER

#define OPEN_READER_NOSIZE(name, gb)            \
			unsigned int name ## _index = (gb)->index;  \
			unsigned int DREAMSKY_UNUSED name ## _cache

#if UNCHECKED_BITSTREAM_READER
#define OPEN_READER(name, gb) OPEN_READER_NOSIZE(name, gb)
#define BITS_AVAILABLE(name, gb) 1
#else
#define OPEN_READER(name, gb)                   \
				OPEN_READER_NOSIZE(name, gb);               \
				unsigned int name ## _size_plus8 = (gb)->size_in_bits_plus8
#define BITS_AVAILABLE(name, gb) name ## _index < name ## _size_plus8
#endif

#define CLOSE_READER(name, gb) (gb)->index = name ## _index

#ifdef LONG_BITSTREAM_READER
#define UPDATE_CACHE_LE(name, gb) name ## _cache = \
			  DREAM_RL64((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)
#define UPDATE_CACHE_BE(name, gb) name ## _cache = \
			  DREAM_RB64((gb)->buffer + (name ## _index >> 3)) >> (32 - (name ## _index & 7))
#else
#define UPDATE_CACHE_LE(name, gb) name ## _cache = \
			  DREAM_RL32((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)
#define UPDATE_CACHE_BE(name, gb) name ## _cache = \
			  DREAM_RB32((gb)->buffer + (name ## _index >> 3)) << (name ## _index & 7)
#endif

#ifdef BITSTREAM_READER_LE
#define UPDATE_CACHE(name, gb) UPDATE_CACHE_LE(name, gb)
#define SKIP_CACHE(name, gb, num) name ## _cache >>= (num)
#else
#define UPDATE_CACHE(name, gb) UPDATE_CACHE_BE(name, gb)
#define SKIP_CACHE(name, gb, num) name ## _cache <<= (num)
#endif

#if UNCHECKED_BITSTREAM_READER
#define SKIP_COUNTER(name, gb, num) name ## _index += (num)
#else
#define SKIP_COUNTER(name, gb, num) \
			name ## _index = DREAM_MIN(name ## _size_plus8, name ## _index + (num))
#endif

#define BITS_LEFT(name, gb) ((int)((gb)->size_in_bits - name ## _index))

#define SKIP_BITS(name, gb, num)                \
		do {                                        \
			SKIP_CACHE(name, gb, num);              \
			SKIP_COUNTER(name, gb, num);            \
		} while (0)

#define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)

#define SHOW_UBITS_LE(name, gb, num) zero_extend(name ## _cache, num)
#define SHOW_SBITS_LE(name, gb, num) sign_extend(name ## _cache, num)

#define SHOW_UBITS_BE(name, gb, num) NEG_USR32(name ## _cache, num)
#define SHOW_SBITS_BE(name, gb, num) NEG_SSR32(name ## _cache, num)

#ifdef BITSTREAM_READER_LE
#define SHOW_UBITS(name, gb, num) SHOW_UBITS_LE(name, gb, num)
#define SHOW_SBITS(name, gb, num) SHOW_SBITS_LE(name, gb, num)
#else
#define SHOW_UBITS(name, gb, num) SHOW_UBITS_BE(name, gb, num)
#define SHOW_SBITS(name, gb, num) SHOW_SBITS_BE(name, gb, num)
#endif

#define GET_CACHE(name, gb) ((uint32_t) name ## _cache)
#endif

#ifndef NEG_SSR32
#   define NEG_SSR32(a,s) ((( int32_t)(a))>>(32-(s)))
#endif

#ifndef NEG_USR32
#   define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#endif

#ifndef sign_extend
static inline DREAMSKY_CONST int sign_extend(int val, unsigned bits)
{
	unsigned shift = 8 * sizeof(int) - bits;
	union { unsigned u; int s; } v = { (unsigned)val << shift };
	return v.s >> shift;
}
#endif

#ifndef zero_extend
static inline DREAMSKY_CONST unsigned zero_extend(unsigned val, unsigned bits)
{
	return (val << ((8 * sizeof(int)) - bits)) >> ((8 * sizeof(int)) - bits);
}
#endif

#if CACHED_BITSTREAM_READER
static inline void refill_32(GetBitContext* s, int is_le)
{
#if !UNCHECKED_BITSTREAM_READER
	if (s->index >> 3 >= s->buffer_end - s->buffer)
		return;
#endif

	if (is_le)
		s->cache = (uint64_t)DREAM_RL32(s->buffer + (s->index >> 3)) << s->bits_left | s->cache;
	else
		s->cache = s->cache | (uint64_t)DREAM_RB32(s->buffer + (s->index >> 3)) << (32 - s->bits_left);
	s->index += 32;
	s->bits_left += 32;
}

static inline void refill_64(GetBitContext* s, int is_le)
{
#if !UNCHECKED_BITSTREAM_READER
	if (s->index >> 3 >= s->buffer_end - s->buffer)
		return;
#endif

	if (is_le)
		s->cache = DREAM_RL64(s->buffer + (s->index >> 3));
	else
		s->cache = DREAM_RB64(s->buffer + (s->index >> 3));
	s->index += 64;
	s->bits_left = 64;
}

static inline uint64_t get_val(GetBitContext* s, unsigned n, int is_le)
{
	uint64_t ret;
	assert(n > 0 && n <= 63);
	if (is_le) {
		ret = s->cache & ((UINT64_C(1) << n) - 1);
		s->cache >>= n;
	}
	else {
		ret = s->cache >> (64 - n);
		s->cache <<= n;
	}
	s->bits_left -= n;
	return ret;
}

static inline unsigned show_val(const GetBitContext* s, unsigned n)
{
#ifdef BITSTREAM_READER_LE
	return s->cache & ((UINT64_C(1) << n) - 1);
#else
	return s->cache >> (64 - n);
#endif
}
#endif

#endif //!__BITS_DEFINE_H__