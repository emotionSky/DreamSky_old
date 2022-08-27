#include "Component/bits_operation/put_bits.h"
#include <cstring>

#ifdef COMPLEX_DEFINES_UNKNOWN
	#define CONFIG_SMALL 1
#else
	#define CONFIG_SMALL 0
#endif

void dreamsky_put_string(PutBitContext* pb, const char* string, int terminate_string)
{
	while (*string) {
		put_bits(pb, 8, *string);
		string++;
	}
	if (terminate_string)
		put_bits(pb, 8, 0);
}

void dreamsky_copy_bits(PutBitContext* pb, const uint8_t* src, int length)
{
	int words = length >> 4;
	int bits = length & 15;
	int i;

	if (length == 0)
		return;

	assert(length <= put_bits_left(pb));

	if (CONFIG_SMALL || words < 16 || put_bits_count(pb) & 7) {
		for (i = 0; i < words; i++)
			put_bits(pb, 16, DREAM_RB16(src + 2 * i));
	}
	else 
	{
		for (i = 0; put_bits_count(pb) & 31; i++)
			put_bits(pb, 8, src[i]);
		flush_put_bits(pb);
		memcpy(put_bits_ptr(pb), src + i, 2 * words - i);
		skip_put_bytes(pb, 2 * words - i);
	}

	put_bits(pb, bits, DREAM_RB16(src + 2 * words) >> (16 - bits));
}