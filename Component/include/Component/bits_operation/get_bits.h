#ifndef __GET_BITS_H__
#define __GET_BITS_H__

#include <Component/bits_operation/bits_define.h>
#include <Component/common/dream_define.h>
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
 * 用于解码的输入比特流末尾需要额外分配的字节数。
 * 因为一些优化的比特流读取器一次读取 32 或 64 位并且可以读取结束，因而需要这个。
 * 注意：如果附加字节的前 23 位不为 0，则损坏的 MPEG 比特流可能导致过度读取和段错误。
 */
#define DREAM_INPUT_BUFFER_PADDING_SIZE 64

/**@struct GetBitContext 
 * @brief 按bit读取buf的上下文对象
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
 * @brief 初始化 GetBitContext.
 * @param[in] buffer   位流缓冲区，必须比实际读取位大 DREAM_INPUT_BUFFER_PADDING_SIZE 个字节，因为一些优化的位流读取器一次读取 32 位或 64 位，并且可以读取结束
 * @param[in] bit_size 缓冲区的大小，以bit为单位
 * @param[in] is_le    是否为小端模式
 * @return
 * - 0  成功
 * - DREAM_ERROR_INVALIDDATA  缓冲区溢出
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
 * @brief 初始化 GetBitContext.
 * @param[in] buffer    位流缓冲区，必须比实际读取位大 DREAM_INPUT_BUFFER_PADDING_SIZE 个字节，因为一些优化的位流读取器一次读取 32 位或 64 位，并且可以读取结束
 * @param[in] bit_size  缓冲区的大小，以bit为单位
 * @return
 * - 0  成功
 * - DREAM_ERROR_INVALIDDATA  缓冲区溢出
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
 * @brief 初始化 GetBitContext.
 * @param[in] buffer    位流缓冲区，必须比实际读取位大 DREAM_INPUT_BUFFER_PADDING_SIZE 个字节，因为一些优化的位流读取器一次读取 32 位或 64 位，并且可以读取结束
 * @param[in] bit_size  缓冲区的大小，以byte为单位
 * @return
 * - 0  成功
 * - DREAM_ERROR_INVALIDDATA  缓冲区溢出
 */
DREAMSKY_API inline int init_get_bits8(GetBitContext* s, const uint8_t* buffer, int byte_size)
{
	if (byte_size > INT_MAX / 8 || byte_size < 0)
		byte_size = -1;
	return init_get_bits(s, buffer, byte_size * 8);
}

/**
 * @brief 初始化 GetBitContext.
 * @param[in] buffer    位流缓冲区，必须比实际读取位大 DREAM_INPUT_BUFFER_PADDING_SIZE 个字节，因为一些优化的位流读取器一次读取 32 位或 64 位，并且可以读取结束
 * @param[in] bit_size  缓冲区的大小，以byte为单位
 * @return
 * - 0  成功
 * - DREAM_ERROR_INVALIDDATA  缓冲区溢出
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
 * @brief 获取当前已经使用的 bits
 * @param[in] s 比特流对象指针
 * @return 已经使用的 bits
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
 * @brief 获取剩余的 bits
 * @param[in] s 比特流对象指针
 * @return 剩余的 bits
 */
DREAMSKY_API inline int get_bits_left(GetBitContext* gb)
{
	return gb->size_in_bits - get_bits_count(gb);
}

/**
 * @brief 读取 MPEG-1 dc-style VLC（符号位 + 尾数，无 MSB）。
 * @details 
 * - 大端模式！！
 * - 如果设置 MSB，则为负数
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  比特流对象指针
 * @param[in] n  bits的数目
 */
DREAMSKY_API inline int get_xbits(GetBitContext* s, int n)
{
#if CACHED_BITSTREAM_READER
	int32_t cache = show_bits(s, 32);
	int sign = ~cache >> 31;
	skip_remaining(s, n);

	return ((((uint32_t)(sign ^ cache)) >> (32 - n)) ^ sign) - sign;
#else
	//使用 register 来声明变量，让其在cpu的寄存器中进行存储，提高访问效率
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
 * @brief 读取 MPEG-1 dc-style VLC（符号位 + 尾数，无 MSB）。
 * @details 
 * - 小端模式！！
 * - 如果设置 MSB，则为负数
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  比特流对象指针
 * @param[in] n  bits的数目
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
 * @brief 读取指定 bits 的数据作为一个 unsigned int 数据
 * @details
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @param[in] n  读取的 bits，必须满足 0 <= n <= 25
 * @return 读取 bits 对应 无符号整形 数据
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
 * @brief 读取一个 bits 的数据作为一个 unsigned int 数据
 * @details
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @return 读取 bits 对应 无符号整形 数据
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
 * @brief 读取指定 bits 的数据作为一个 unsigned int 数据
 * @details
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @param[in] n  读取的 bits，必须满足 0 <= n <= 32
 * @return 读取 bits 对应 无符号整形 数据
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
 * @brief 读取指定 bits 的数据作为一个 unsigned int 数据
 * @details
 * - 大端模式！！
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @param[in] n  读取的 bits，必须满足 0 <= n <= 64
 * @return 读取 bits 对应 无符号整形 数据
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
 * @brief 读取指定 bits 的数据作为一个 signed int 数据
 * @details
 * - 大端模式！！
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @param[in] n  读取的 bits，必须满足 0 <= n <= 25
 * @return 读取 bits 对应 有符号整形 数据
 */
DREAMSKY_API DREAMSKY_ALWAYS_INLINE int get_bitsz(GetBitContext* s, int n)
{
	return n ? get_bits(s, n) : 0;
}

/**
 * @brief 读取指定 bits 的数据作为一个 unsigned int 数据
 * @details
 * - 小端模式！！
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @param[in] n  读取的 bits，必须满足 0 <= n <= 25
 * @return 读取 bits 对应无符号整形数据
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
 * @brief 读取指定 bits 的数据作为一个 signed int 数据
 * @details
 * - 大端模式！！
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @param[in] n  读取的 bits，必须满足 0 <= n <= 25
 * @return 读取 bits 对应 有符号整形 数据
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
 * @brief 读取指定 bits 的数据作为一个 signed int 数据
 * @details
 * - 大端模式！！
 * - 对应 会 在比特流对象中更改 bit 的索引
 * @param[in] s  读取的 比特流对象指针
 * @param[in] n  读取的 bits，必须满足 0 <= n <= 32
 * @return 读取 bits 对应 有符号整形 数据
 */
DREAMSKY_API inline int get_sbits_long(GetBitContext* s, int n)
{
	// sign_extend(x, 0) is undefined
	if (!n)
		return 0;

	return sign_extend(get_bits_long(s, n), n);
}

/**
 * @brief 展示比特位对应的数据
 * @details
 * - 对应 不会 在比特流对象中更改 bit 的索引
 * @param[in] s  比特流对象指针
 * @param[in] n  展示的 bits，必须满足 0 <= n <= 25
 * @return 展示的对应数据的 无符号整形 数据
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
 * @brief 展示1bit对应的数据
 * @details
 * - 大端模式！！
 * - 对应 不会 在比特流对象中更改 bit 的索引
 * @param[in] s  比特流对象指针
 * @return 展示的对应数据的 无符号整形 数据
 */
DREAMSKY_API inline unsigned int show_bits1(GetBitContext* s)
{
	return show_bits(s, 1);
}

/**
 * @brief 展示比特位对应的数据
 * @details
 * - 大端模式！！
 * - 对应 不会 在比特流对象中更改 bit 的索引
 * @param[in] s  比特流对象指针
 * @param[in] n  展示的 bits，必须满足 0 <= n <= 32
 * @return 展示的对应数据的 无符号整形 数据
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
 * @brief 跳过指定的 bits
 * @param[in] s  比特流对象指针
 * @param[in] n  跳过的 bits 数目
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
 * @brief 跳过一个 bit
 * @param[in] s  比特流对象指针
 * @return void
 */
DREAMSKY_API inline void skip_bits1(GetBitContext* s)
{
	skip_bits(s, 1);
}

/**
 * @brief 跳过指定数目的bits
 * @param[in] s bits对象指针
 * @param[in] n 需要跳过的bits数目，需要保证没有溢出 int32_t
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
 * @brief 对齐字节
 * @param[in] s bits对象指针
 * @return 对其字节后的缓冲区指针
 */
DREAMSKY_API inline const uint8_t* align_get_bits(GetBitContext* s)
{
	int n = -get_bits_count(s) & 7;
	if (n)
		skip_bits(s, n);
	return s->buffer + (s->index >> 3);
}

/* 返回给定比特流配置的 LUT 元素。 */
DREAMSKY_API inline int set_idx(GetBitContext* s, int code, int* n, int* nb_bits, const VLCElem* table)
{
	unsigned idx;

	*nb_bits = -*n;
	idx = show_bits(s, *nb_bits) + code;
	*n = table[idx].len;

	return table[idx].sym;
}

/**
 * 如果 vlc 代码无效且 max_depth=1，则不会删除任何位。
 * 如果 vlc 代码无效且 max_depth>1，则移除的位数未定义。
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
 * @brief 解析 vlc 代码
 * @param[in] bits      一次读取的位数，必须与 init_vlc() 中的 nb_bits 相同
 * @param[in] max_depth 要完全读取最长的 vlc 代码，必须读取 bits bits 的次数 = (max_vlc_length + bits - 1) / bits
 * @return
 * - -1 没有匹配的vlc
 * - 其他 解析的代码
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