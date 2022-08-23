#ifndef __GET_BITS_H__
#define __GET_BITS_H__

#include "common_type.h"
#include "Component/bits_operation/bits_define.h"
#include "Component/common/common_define.h"
#include <cstdint>
#include <cassert>
#include <climits>

#ifndef UNCHECKED_BITSTREAM_READER
#define UNCHECKED_BITSTREAM_READER !CONFIG_SAFE_BITSTREAM_READER
#endif

#ifndef CACHED_BITSTREAM_READER
#define CACHED_BITSTREAM_READER 0
#endif

/**
 * ���ڽ�������������ĩβ��Ҫ���������ֽ�����
 * ��ΪһЩ�Ż��ı�������ȡ��һ�ζ�ȡ 32 �� 64 λ���ҿ��Զ�ȡ�����������Ҫ�����
 * ע�⣺��������ֽڵ�ǰ 23 λ��Ϊ 0�����𻵵� MPEG ���������ܵ��¹��ȶ�ȡ�Ͷδ���
 */
#define DREAM_INPUT_BUFFER_PADDING_SIZE 64

/**@struct GetBitContext 
 * @brief ��bit��ȡbuf�������Ķ���
 */
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
 * @brief ��ʼ�� GetBitContext.
 * @param[in] buffer   λ���������������ʵ�ʶ�ȡλ�� DREAM_INPUT_BUFFER_PADDING_SIZE ���ֽڣ���ΪһЩ�Ż���λ����ȡ��һ�ζ�ȡ 32 λ�� 64 λ�����ҿ��Զ�ȡ����
 * @param[in] bit_size �������Ĵ�С����bitΪ��λ
 * @param[in] is_le    �Ƿ�ΪС��ģʽ
 * @return
 * - 0  �ɹ�
 * - DREAM_ERROR_INVALIDDATA  ���������
 */
DREAMSKY_API inline int init_get_bits_xe(GetBitContext* s, const uint8_t* buffer, int bit_size, int is_le)
{
	int buffer_size;
	int ret = 0;

	if (bit_size >= INT_MAX - DREAM_MAX(7, DREAM_INPUT_BUFFER_PADDING_SIZE * 8) || bit_size < 0 || !buffer)
	{
		bit_size = 0;
		buffer = nullptr;
		ret = DREAM_ERROR_INVALIDDATA;
	}

	buffer_size = (bit_size + 7) >> 3;

	s->buffer = buffer;
	s->size_in_bits = bit_size;
	s->size_in_bits_plus8 = bit_size + 8;
	s->buffer_end = buffer + buffer_size;
	s->index = 0;

#if CACHED_BITSTREAM_READER
	s->cache = 0;
	s->bits_left = 0;
	refill_64(s, is_le);
#endif

	return ret;
}

/**
 * @brief ��ʼ�� GetBitContext.
 * @param[in] buffer    λ���������������ʵ�ʶ�ȡλ�� DREAM_INPUT_BUFFER_PADDING_SIZE ���ֽڣ���ΪһЩ�Ż���λ����ȡ��һ�ζ�ȡ 32 λ�� 64 λ�����ҿ��Զ�ȡ����
 * @param[in] bit_size  �������Ĵ�С����bitΪ��λ
 * @return
 * - 0  �ɹ�
 * - DREAM_ERROR_INVALIDDATA  ���������
 */
DREAMSKY_API inline int init_get_bits(GetBitContext* s, const uint8_t* buffer, int bit_size)
{
#ifdef BITSTREAM_READER_LE
	return init_get_bits_xe(s, buffer, bit_size, 1);
#else
	return init_get_bits_xe(s, buffer, bit_size, 0);
#endif
}

/**
 * @brief ��ʼ�� GetBitContext.
 * @param[in] buffer    λ���������������ʵ�ʶ�ȡλ�� DREAM_INPUT_BUFFER_PADDING_SIZE ���ֽڣ���ΪһЩ�Ż���λ����ȡ��һ�ζ�ȡ 32 λ�� 64 λ�����ҿ��Զ�ȡ����
 * @param[in] bit_size  �������Ĵ�С����byteΪ��λ
 * @return
 * - 0  �ɹ�
 * - DREAM_ERROR_INVALIDDATA  ���������
 */
DREAMSKY_API inline int init_get_bits8(GetBitContext* s, const uint8_t* buffer, int byte_size)
{
	if (byte_size > INT_MAX / 8 || byte_size < 0)
		byte_size = -1;
	return init_get_bits(s, buffer, byte_size * 8);
}

/**
 * @brief ��ʼ�� GetBitContext.
 * @param[in] buffer    λ���������������ʵ�ʶ�ȡλ�� DREAM_INPUT_BUFFER_PADDING_SIZE ���ֽڣ���ΪһЩ�Ż���λ����ȡ��һ�ζ�ȡ 32 λ�� 64 λ�����ҿ��Զ�ȡ����
 * @param[in] bit_size  �������Ĵ�С����byteΪ��λ
 * @return
 * - 0  �ɹ�
 * - DREAM_ERROR_INVALIDDATA  ���������
 */
DREAMSKY_API inline int init_get_bits8_le(GetBitContext* s, const uint8_t* buffer, int byte_size)
{
	if (byte_size > INT_MAX / 8 || byte_size < 0)
		byte_size = -1;
	return init_get_bits_xe(s, buffer, byte_size * 8, 1);
}

#if CACHED_BITSTREAM_READER
static inline void skip_remaining(GetBitContext* s, unsigned n)
{
#ifdef BITSTREAM_READER_LE
	s->cache >>= n;
#else
	s->cache <<= n;
#endif
	s->bits_left -= n;
}
#endif

/**
 * @brief ��ȡ��ǰ�Ѿ�ʹ�õ� bits
 * @param[in] s ����������ָ��
 * @return �Ѿ�ʹ�õ� bits
 */
DREAMSKY_API inline int get_bits_count(const GetBitContext* s)
{
#if CACHED_BITSTREAM_READER
    return s->index - s->bits_left;
#else
    return s->index;
#endif
}

/**
 * @brief ��ȡʣ��� bits
 * @param[in] s ����������ָ��
 * @return ʣ��� bits
 */
DREAMSKY_API inline int get_bits_left(GetBitContext* gb)
{
	return gb->size_in_bits - get_bits_count(gb);
}

/**
 * @brief ��ȡ MPEG-1 dc-style VLC������λ + β������ MSB����
 * @details 
 * - ���ģʽ����
 * - ������� MSB����Ϊ����
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ����������ָ��
 * @param[in] n  bits����Ŀ
 */
DREAMSKY_API inline int get_xbits(GetBitContext* s, int n)
{
#if CACHED_BITSTREAM_READER
	int32_t cache = show_bits(s, 32);
	int sign = ~cache >> 31;
	skip_remaining(s, n);

	return ((((uint32_t)(sign ^ cache)) >> (32 - n)) ^ sign) - sign;
#else
	//ʹ�� register ������������������cpu�ļĴ����н��д洢����߷���Ч��
	register int sign;
	register int32_t cache;
	OPEN_READER(re, s);
	assert(n > 0 && n <= 25);
	UPDATE_CACHE(re, s);
	cache = GET_CACHE(re, s);
	sign = ~cache >> 31;
	LAST_SKIP_BITS(re, s, n);
	CLOSE_READER(re, s);
	return (NEG_USR32(sign ^ cache, n) ^ sign) - sign;
#endif
}

#if !CACHED_BITSTREAM_READER
/**
 * @brief ��ȡ MPEG-1 dc-style VLC������λ + β������ MSB����
 * @details 
 * - С��ģʽ����
 * - ������� MSB����Ϊ����
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ����������ָ��
 * @param[in] n  bits����Ŀ
 */
DREAMSKY_API inline int get_xbits_le(GetBitContext* s, int n)
{
    register int sign;
    register int32_t cache;
    OPEN_READER(re, s);
    assert(n > 0 && n <= 25);
    UPDATE_CACHE_LE(re, s);
    cache = GET_CACHE(re, s);
    sign = sign_extend(~cache, n) >> 31;
    LAST_SKIP_BITS(re, s, n);
    CLOSE_READER(re, s);
    return (zero_extend(sign ^ cache, n) ^ sign) - sign;
}
#endif

/**
 * @brief ��ȡָ�� bits ��������Ϊһ�� unsigned int ����
 * @details
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @param[in] n  ��ȡ�� bits���������� 0 <= n <= 25
 * @return ��ȡ bits ��Ӧ �޷������� ����
 */
DREAMSKY_API inline unsigned int get_bits(GetBitContext* s, int n)
{
	register unsigned int tmp;
#if CACHED_BITSTREAM_READER

	assert(n > 0 && n <= 32);
	if (n > s->bits_left)
	{
#ifdef BITSTREAM_READER_LE
		refill_32(s, 1);
#else
		refill_32(s, 0);
#endif
		if (s->bits_left < 32)
			s->bits_left = n;
	}

#ifdef BITSTREAM_READER_LE
	tmp = get_val(s, n, 1);
#else
	tmp = get_val(s, n, 0);
#endif
#else
	OPEN_READER(re, s);
	assert(n > 0 && n <= 25);
	UPDATE_CACHE(re, s);
	tmp = SHOW_UBITS(re, s, n);
	LAST_SKIP_BITS(re, s, n);
	CLOSE_READER(re, s);
#endif
	assert(tmp < UINT64_C(1) << n);
	return tmp;
}

/**
 * @brief ��ȡһ�� bits ��������Ϊһ�� unsigned int ����
 * @details
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @return ��ȡ bits ��Ӧ �޷������� ����
 */
DREAMSKY_API inline unsigned int get_bits1(GetBitContext* s)
{
#if CACHED_BITSTREAM_READER
	if (!s->bits_left)
#ifdef BITSTREAM_READER_LE
		refill_64(s, 1);
#else
		refill_64(s, 0);
#endif

#ifdef BITSTREAM_READER_LE
	return get_val(s, 1, 1);
#else
	return get_val(s, 1, 0);
#endif
#else
	unsigned int index = s->index;
	uint8_t result = s->buffer[index >> 3];
#ifdef BITSTREAM_READER_LE
	result >>= index & 7;
	result &= 1;
#else
	result <<= index & 7;
	result >>= 8 - 1;
#endif
#if !UNCHECKED_BITSTREAM_READER
	if (s->index < s->size_in_bits_plus8)
#endif
		index++;
	s->index = index;

	return result;
#endif
}

/**
 * @brief ��ȡָ�� bits ��������Ϊһ�� unsigned int ����
 * @details
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @param[in] n  ��ȡ�� bits���������� 0 <= n <= 32
 * @return ��ȡ bits ��Ӧ �޷������� ����
 */
DREAMSKY_API inline unsigned int get_bits_long(GetBitContext* s, int n)
{
	assert(n >= 0 && n <= 32);
	if (!n)
	{
		return 0;
#if CACHED_BITSTREAM_READER
	}
	return get_bits(s, n);
#else
}
	else if (n <= MIN_CACHE_BITS)
	{
		return get_bits(s, n);
	}
	else
	{
#ifdef BITSTREAM_READER_LE
		unsigned ret = get_bits(s, 16);
		return ret | (get_bits(s, n - 16) << 16);
#else
		unsigned ret = get_bits(s, 16) << (n - 16);
		return ret | get_bits(s, n - 16);
#endif
	}
#endif
}

/**
 * @brief ��ȡָ�� bits ��������Ϊһ�� unsigned int ����
 * @details
 * - ���ģʽ����
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @param[in] n  ��ȡ�� bits���������� 0 <= n <= 64
 * @return ��ȡ bits ��Ӧ �޷������� ����
 */
DREAMSKY_API inline uint64_t get_bits64(GetBitContext* s, int n)
{
    if (n <= 32)
    {
        return get_bits_long(s, n);
    }
    else
    {
#ifdef BITSTREAM_READER_LE
        uint64_t ret = get_bits_long(s, 32);
        return ret | (uint64_t)get_bits_long(s, n - 32) << 32;
#else
        uint64_t ret = (uint64_t)get_bits_long(s, n - 32) << 32;
        return ret | get_bits_long(s, 32);
#endif
    }
}

/**
 * @brief ��ȡָ�� bits ��������Ϊһ�� signed int ����
 * @details
 * - ���ģʽ����
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @param[in] n  ��ȡ�� bits���������� 0 <= n <= 25
 * @return ��ȡ bits ��Ӧ �з������� ����
 */
DREAMSKY_API DREAMSKY_ALWAYS_INLINE int get_bitsz(GetBitContext* s, int n)
{
	return n ? get_bits(s, n) : 0;
}

/**
 * @brief ��ȡָ�� bits ��������Ϊһ�� unsigned int ����
 * @details
 * - С��ģʽ����
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @param[in] n  ��ȡ�� bits���������� 0 <= n <= 25
 * @return ��ȡ bits ��Ӧ�޷�����������
 */
DREAMSKY_API inline unsigned int get_bits_le(GetBitContext* s, int n)
{
#if CACHED_BITSTREAM_READER
	assert(n > 0 && n <= 32);
	if (n > s->bits_left)
	{
		refill_32(s, 1);
		if (s->bits_left < 32)
			s->bits_left = n;
	}

	return get_val(s, n, 1);
#else
	register int tmp;
	OPEN_READER(re, s);
	assert(n > 0 && n <= 25);
	UPDATE_CACHE_LE(re, s);
	tmp = SHOW_UBITS_LE(re, s, n);
	LAST_SKIP_BITS(re, s, n);
	CLOSE_READER(re, s);
	return tmp;
#endif
}

/**
 * @brief ��ȡָ�� bits ��������Ϊһ�� signed int ����
 * @details
 * - ���ģʽ����
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @param[in] n  ��ȡ�� bits���������� 0 <= n <= 25
 * @return ��ȡ bits ��Ӧ �з������� ����
 */
DREAMSKY_API inline int get_sbits(GetBitContext* s, int n)
{
	register int tmp;
#if CACHED_BITSTREAM_READER
	assert(n > 0 && n <= 25);
	tmp = sign_extend(get_bits(s, n), n);
#else
	OPEN_READER(re, s);
	assert(n > 0 && n <= 25);
	UPDATE_CACHE(re, s);
	tmp = SHOW_SBITS(re, s, n);
	LAST_SKIP_BITS(re, s, n);
	CLOSE_READER(re, s);
#endif
	return tmp;
}

/**
 * @brief ��ȡָ�� bits ��������Ϊһ�� signed int ����
 * @details
 * - ���ģʽ����
 * - ��Ӧ �� �ڱ����������и��� bit ������
 * @param[in] s  ��ȡ�� ����������ָ��
 * @param[in] n  ��ȡ�� bits���������� 0 <= n <= 32
 * @return ��ȡ bits ��Ӧ �з������� ����
 */
DREAMSKY_API inline int get_sbits_long(GetBitContext* s, int n)
{
	// sign_extend(x, 0) is undefined
	if (!n)
		return 0;

	return sign_extend(get_bits_long(s, n), n);
}

/**
 * @brief չʾ����λ��Ӧ������
 * @details
 * - ��Ӧ ���� �ڱ����������и��� bit ������
 * @param[in] s  ����������ָ��
 * @param[in] n  չʾ�� bits���������� 0 <= n <= 25
 * @return չʾ�Ķ�Ӧ���ݵ� �޷������� ����
 */
DREAMSKY_API inline unsigned int show_bits(GetBitContext* s, int n)
{
	register unsigned int tmp;
#if CACHED_BITSTREAM_READER
	if (n > s->bits_left)
#ifdef BITSTREAM_READER_LE
		refill_32(s, 1);
#else
		refill_32(s, 0);
#endif

	tmp = show_val(s, n);
#else
	OPEN_READER_NOSIZE(re, s);
	assert(n > 0 && n <= 25);
	UPDATE_CACHE(re, s);
	tmp = SHOW_UBITS(re, s, n);
#endif
	return tmp;
}

/**
 * @brief չʾ1bit��Ӧ������
 * @details
 * - ���ģʽ����
 * - ��Ӧ ���� �ڱ����������и��� bit ������
 * @param[in] s  ����������ָ��
 * @return չʾ�Ķ�Ӧ���ݵ� �޷������� ����
 */
DREAMSKY_API inline unsigned int show_bits1(GetBitContext* s)
{
	return show_bits(s, 1);
}

/**
 * @brief չʾ����λ��Ӧ������
 * @details
 * - ���ģʽ����
 * - ��Ӧ ���� �ڱ����������и��� bit ������
 * @param[in] s  ����������ָ��
 * @param[in] n  չʾ�� bits���������� 0 <= n <= 32
 * @return չʾ�Ķ�Ӧ���ݵ� �޷������� ����
 */
DREAMSKY_API inline unsigned int show_bits_long(GetBitContext* s, int n)
{
    if (n <= MIN_CACHE_BITS)
    {
        return show_bits(s, n);
    }
    else
    {
        GetBitContext gb = *s;
        return get_bits_long(&gb, n);
    }
}

/**
 * @brief ����ָ���� bits
 * @param[in] s  ����������ָ��
 * @param[in] n  ������ bits ��Ŀ
 * @return void
 */
DREAMSKY_API inline void skip_bits(GetBitContext* s, int n)
{
#if CACHED_BITSTREAM_READER
	if (n < s->bits_left)
		skip_remaining(s, n);
	else
	{
		n -= s->bits_left;
		s->cache = 0;
		s->bits_left = 0;

		if (n >= 64)
		{
			unsigned skip = (n / 8) * 8;

			n -= skip;
			s->index += skip;
		}
#ifdef BITSTREAM_READER_LE
		refill_64(s, 1);
#else
		refill_64(s, 0);
#endif
		if (n)
			skip_remaining(s, n);
	}
#else
	OPEN_READER(re, s);
	LAST_SKIP_BITS(re, s, n);
	CLOSE_READER(re, s);
#endif
}

/**
 * @brief ����һ�� bit
 * @param[in] s  ����������ָ��
 * @return void
 */
DREAMSKY_API inline void skip_bits1(GetBitContext* s)
{
	skip_bits(s, 1);
}

/**
 * @brief ����ָ����Ŀ��bits
 * @param[in] s bits����ָ��
 * @param[in] n ��Ҫ������bits��Ŀ����Ҫ��֤û����� int32_t
 */
DREAMSKY_API inline void skip_bits_long(GetBitContext* s, int n)
{
#if CACHED_BITSTREAM_READER
    skip_bits(s, n);
#else
#if UNCHECKED_BITSTREAM_READER
    s->index += n;
#else
    s->index += av_clip(n, -s->index, s->size_in_bits_plus8 - s->index);
#endif
#endif
}

DREAMSKY_API inline int decode012(GetBitContext* gb)
{
	int n;
	n = get_bits1(gb);
	if (n == 0)
		return 0;
	else
		return get_bits1(gb) + 1;
}

DREAMSKY_API inline int decode210(GetBitContext* gb)
{
	if (get_bits1(gb))
		return 0;
	else
		return 2 - get_bits1(gb);
}

DREAMSKY_API inline int skip_1stop_8data_bits(GetBitContext* gb)
{
	if (get_bits_left(gb) <= 0)
		return DREAM_ERROR_INVALIDDATA;

	while (get_bits1(gb))
	{
		skip_bits(gb, 8);
		if (get_bits_left(gb) <= 0)
			return DREAM_ERROR_INVALIDDATA;
	}

	return 0;
}

/**
 * @brief �����ֽ�
 * @param[in] s bits����ָ��
 * @return �����ֽں�Ļ�����ָ��
 */
DREAMSKY_API inline const uint8_t* align_get_bits(GetBitContext* s)
{
	int n = -get_bits_count(s) & 7;
	if (n)
		skip_bits(s, n);
	return s->buffer + (s->index >> 3);
}

/* ���ظ������������õ� LUT Ԫ�ء� */
DREAMSKY_API inline int set_idx(GetBitContext* s, int code, int* n, int* nb_bits, const VLCElem* table)
{
	unsigned idx;

	*nb_bits = -*n;
	idx = show_bits(s, *nb_bits) + code;
	*n = table[idx].len;

	return table[idx].sym;
}

/**
 * ��� vlc ������Ч�� max_depth=1���򲻻�ɾ���κ�λ��
 * ��� vlc ������Ч�� max_depth>1�����Ƴ���λ��δ���塣
 */
#define GET_VLC(code, name, gb, table, bits, max_depth)         \
    do {                                                        \
        int n, nb_bits;                                         \
        unsigned int index;                                     \
                                                                \
        index = SHOW_UBITS(name, gb, bits);                     \
        code  = table[index].sym;                               \
        n     = table[index].len;                               \
                                                                \
        if (max_depth > 1 && n < 0) {                           \
            LAST_SKIP_BITS(name, gb, bits);                     \
            UPDATE_CACHE(name, gb);                             \
                                                                \
            nb_bits = -n;                                       \
                                                                \
            index = SHOW_UBITS(name, gb, nb_bits) + code;       \
            code  = table[index].sym;                           \
            n     = table[index].len;                           \
            if (max_depth > 2 && n < 0) {                       \
                LAST_SKIP_BITS(name, gb, nb_bits);              \
                UPDATE_CACHE(name, gb);                         \
                                                                \
                nb_bits = -n;                                   \
                                                                \
                index = SHOW_UBITS(name, gb, nb_bits) + code;   \
                code  = table[index].sym;                       \
                n     = table[index].len;                       \
            }                                                   \
        }                                                       \
        SKIP_BITS(name, gb, n);                                 \
    } while (0)

#define GET_RL_VLC(level, run, name, gb, table, bits,  \
                   max_depth, need_update)                      \
    do {                                                        \
        int n, nb_bits;                                         \
        unsigned int index;                                     \
                                                                \
        index = SHOW_UBITS(name, gb, bits);                     \
        level = table[index].level;                             \
        n     = table[index].len;                               \
                                                                \
        if (max_depth > 1 && n < 0) {                           \
            SKIP_BITS(name, gb, bits);                          \
            if (need_update) {                                  \
                UPDATE_CACHE(name, gb);                         \
            }                                                   \
                                                                \
            nb_bits = -n;                                       \
                                                                \
            index = SHOW_UBITS(name, gb, nb_bits) + level;      \
            level = table[index].level;                         \
            n     = table[index].len;                           \
            if (max_depth > 2 && n < 0) {                       \
                LAST_SKIP_BITS(name, gb, nb_bits);              \
                if (need_update) {                              \
                    UPDATE_CACHE(name, gb);                     \
                }                                               \
                nb_bits = -n;                                   \
                                                                \
                index = SHOW_UBITS(name, gb, nb_bits) + level;  \
                level = table[index].level;                     \
                n     = table[index].len;                       \
            }                                                   \
        }                                                       \
        run = table[index].run;                                 \
        SKIP_BITS(name, gb, n);                                 \
    } while (0)


/**
 * @brief ���� vlc ����
 * @param[in] bits      һ�ζ�ȡ��λ���������� init_vlc() �е� nb_bits ��ͬ
 * @param[in] max_depth Ҫ��ȫ��ȡ��� vlc ���룬�����ȡ bits bits �Ĵ��� = (max_vlc_length + bits - 1) / bits
 * @return
 * - -1 û��ƥ���vlc
 * - ���� �����Ĵ���
 */
DREAMSKY_API DREAMSKY_ALWAYS_INLINE int get_vlc2(GetBitContext* s, const VLCElem* table, int bits, int max_depth)
{
#if CACHED_BITSTREAM_READER
	int nb_bits;
	unsigned idx = show_bits(s, bits);
	int code = table[idx].sym;
	int n = table[idx].len;

	if (max_depth > 1 && n < 0)
	{
		skip_remaining(s, bits);
		code = set_idx(s, code, &n, &nb_bits, table);
		if (max_depth > 2 && n < 0)
		{
			skip_remaining(s, nb_bits);
			code = set_idx(s, code, &n, &nb_bits, table);
		}
	}
	skip_remaining(s, n);

	return code;
#else
	int code;

	OPEN_READER(re, s);
	UPDATE_CACHE(re, s);

	GET_VLC(code, re, s, table, bits, max_depth);

	CLOSE_READER(re, s);

	return code;
#endif
}

#endif //!__GET_BITS_H__