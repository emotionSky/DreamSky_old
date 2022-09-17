#include "Algorithm/EnDecode/base_xx.h"
#include "Component/bits_operation/get_bits.h"

static const uint8_t base16_code[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static inline uint8_t base16_value(uint8_t ch)
{
	if ('0' <= ch <= '9')
		return ch - '0';

	if ('A' <= ch <= 'F')
		return ch - 'A';

#ifdef ALLOW_LITTLE_CH
	if ('a' <= ch <= 'f')
		return ch - 'a';
#endif

	return 0;
}

int base16_encode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len)
{
	if (!src || !dst || !dst_len)
		return BASE_XX_ERROR;

	*dst_len = 0;
	for (int index = 0; index < src_len; ++index)
	{
		dst[(*dst_len)++] = base16_code[src[index] >> 4];
		dst[(*dst_len)++] = base16_code[src[index] & 0x0f];
	}

	return BASE_XX_SUCCESS;
}

int base16_decode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len)
{
	if (!src || !dst || !dst_len /* || (src_len % 2) != 0*/)
		return BASE_XX_ERROR;

	*dst_len = 0;
	for (int index = 0; index < src_len; index += 2)
	{
		dst[*dst_len] = base16_value(src[index]);
		dst[*dst_len] <<= 4;
		dst[*dst_len] |= base16_value(src[index + 1]);
		++* dst_len;
	}

	return BASE_XX_SUCCESS;
}

static uint8_t base32_code[] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 
	'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z',
	'2', '3', '4', '5', '6', '7'
};

#define BASE32_STUFF '='

int base32_encode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len)
{
	if (!src || !dst || !dst_len)
		return BASE_XX_ERROR;

	GetBitContext g_ctx;
	GetBitContext* gb = &g_ctx;
	init_get_bits8(gb, src, src_len);

	//TODO

	return BASE_XX_SUCCESS;
}

int base32_decode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len) 
{
	if (!src || !dst || !dst_len)
		return BASE_XX_ERROR;

	//TODO

	return BASE_XX_SUCCESS;
}

int base64_encode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len) 
{
	if (!src || !dst || !dst_len)
		return BASE_XX_ERROR;

	//TODO

	return BASE_XX_SUCCESS;
}

int base64_decode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len) 
{
	if (!src || !dst || !dst_len)
		return BASE_XX_ERROR;

	//TODO

	return BASE_XX_SUCCESS;
}