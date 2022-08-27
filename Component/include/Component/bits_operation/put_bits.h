#ifndef __PUT_BITS_H__
#define __PUT_BITS_H__

#include "common_type.h"
#include "Component/common/common_define.h"
#include "Component/bits_operation/int_read_write.h"
#include <cassert>
#include <climits>

#if ARCH_64BITS
typedef uint64_t BitBuf;
#define DREAM_WBBUF DREAM_WB64
#define DREAM_WLBUF DREAM_WL64
#else
typedef uint32_t BitBuf;
#define DREAM_WBBUF DREAM_WB32
#define DREAM_WLBUF DREAM_WL32
#endif

//一个buf有多少个bits
static const int BUF_BITS = 8 * sizeof(BitBuf);

/**@struct PutBitContext
 * @brief 按bit写入buf的上下文对象
 */
struct PutBitContext 
{
	BitBuf bit_buf;
	int bit_left;
	uint8_t* buf, * buf_ptr, * buf_end;
};

/**
 * @brief 初始化 PutBitContext 对象
 * @param[in] s            写入的比特流对象指针
 * @param[in] buffer       将要写入 bit 的buf指针
 * @param[in] buffer_size  buf 的大小(字节数)
 * @return void
 */
DREAMSKY_API inline void init_put_bits(PutBitContext* s, uint8_t* buffer, int buffer_size)
{
    if (buffer_size < 0) 
    {
        buffer_size = 0;
        buffer = nullptr;
    }

    s->buf = buffer;
    s->buf_end = s->buf + buffer_size;
    s->buf_ptr = s->buf;
    s->bit_left = BUF_BITS;
    s->bit_buf = 0;
}

/**
 * @brief 获取已经写入比特流对象中 bits 数目
 * @param[in] s    写入的比特流对象指针
 * @return 写入的 bits 数目
 */
DREAMSKY_API inline int put_bits_count(PutBitContext* s)
{
    return (s->buf_ptr - s->buf) * 8 + BUF_BITS - s->bit_left;
}

/**
 * @brief 获取目前为止输出的字节数目
 * @detail 只能在新初始化或刷新 PutBitContext 时调用。
 * @param[in] s    写入的比特流对象指针
 * @return 输出的字节数目
 */
DREAMSKY_API inline int put_bytes_output(const PutBitContext* s)
{
    assert(s->bit_left == BUF_BITS);
    return s->buf_ptr - s->buf;
}

/**
 * @brief 获取目前为止输出的字节数目
 * @detail 可以任意时刻使用 
 * @param[in] s         写入的比特流对象指针
 * @param[in] round_up  设置后，到目前为止写入的位数将向上舍入到下一个字节。
 * @return  目前为止输出的字节数目
 */
DREAMSKY_API inline int put_bytes_count(const PutBitContext* s, int round_up)
{
    return s->buf_ptr - s->buf + ((BUF_BITS - s->bit_left + (round_up ? 7 : 0)) >> 3);
}

/**
 * @brief 将比特流对象重置到新的 buf
 * @param[in] s            写入的比特流对象指针
 * @param[in] buffer       将要写入 bit 的buf指针
 * @param[in] buffer_size  buf 的大小(字节数)
 * @return void
 */
DREAMSKY_API inline void rebase_put_bits(PutBitContext* s, uint8_t* buffer, int buffer_size)
{
    assert(8 * buffer_size >= put_bits_count(s));

    s->buf_end = buffer + buffer_size;
    s->buf_ptr = buffer + (s->buf_ptr - s->buf);
    s->buf = buffer;
}

/**
 * @brief 获取比特流对象中还剩余的 bits 数目
 * @param[in] s         写入的比特流对象指针
 * @return  可使用的 bits 数目
 */
DREAMSKY_API inline int put_bits_left(PutBitContext* s)
{
    return (s->buf_end - s->buf_ptr) * 8 - BUF_BITS + s->bit_left;
}

/**
 * @brief 获取剩余的字节数目
 * @param[in] s         写入的比特流对象指针
 * @param[in] round_up  设置后，写入的位数将向上舍入到下一个字节。
 * @return 剩余字节数目
 */
DREAMSKY_API inline int put_bytes_left(const PutBitContext* s, int round_up)
{
    return s->buf_end - s->buf_ptr - ((BUF_BITS - s->bit_left + (round_up ? 7 : 0)) >> 3);
}

/**
 * @brief flush 比特流对象
 * @detail 用 0 填充还未填入的部分
 * @param[in] s         写入的比特流对象指针
 * @return void
 */
DREAMSKY_API inline void flush_put_bits(PutBitContext* s)
{
#ifndef BITSTREAM_WRITER_LE
    if (s->bit_left < BUF_BITS)
        s->bit_buf <<= s->bit_left;
#endif
    while (s->bit_left < BUF_BITS) 
    {
        assert(s->buf_ptr < s->buf_end);
#ifdef BITSTREAM_WRITER_LE
        * s->buf_ptr++ = s->bit_buf;
        s->bit_buf >>= 8;
#else
        * s->buf_ptr++ = s->bit_buf >> (BUF_BITS - 8);
        s->bit_buf <<= 8;
#endif
        s->bit_left += 8;
    }
    s->bit_left = BUF_BITS;
    s->bit_buf = 0;
}

/**
 * @brief flush 比特流对象
 * @detail 用 0 填充还未填入的部分
 * @param[in] s         写入的比特流对象指针
 * @return void
 */
DREAMSKY_API inline void flush_put_bits_le(PutBitContext* s)
{
    while (s->bit_left < BUF_BITS) 
    {
        assert(s->buf_ptr < s->buf_end);
        *s->buf_ptr++ = s->bit_buf;
        s->bit_buf >>= 8;
        s->bit_left += 8;
    }
    s->bit_left = BUF_BITS;
    s->bit_buf = 0;
}

#ifdef BITSTREAM_WRITER_LE
#define dreamsky_put_string dreamsky_put_string_unsupported_here
#define dreamsky_copy_bits dreamsky_copy_bits_unsupported_here
#else

/**
 * @brief 将字符串放入到比特流对象中
 * @param[in] pb                写入的比特流对象指针
 * @param[in] string            将要写入的字符串
 * @param[in] terminate_string  如果设置为1，那么将会在字符串写完之后自动补一个字符串结束符
 * @return void
 */
void dreamsky_put_string(PutBitContext* pb, const char* string, int terminate_string);

/**
 * @brief 将一个 buf 中的数据拷贝到比特流对象中
 * @param[in] pb      写入的比特流对象指针
 * @param[in] src     将要拷贝的 buf
 * @param[in] length  拷贝的 bits 数目
 * @return void
 */
void dreamsky_copy_bits(PutBitContext* pb, const uint8_t* src, int length);
#endif

/**
 * @brief 将数值按照 bit 写入到比特流对象中
 * @details 没有异常抛出
 * @param[in] s      写入的比特流对象指针
 * @param[in] n      写入的bits数目
 * @param[in] value  写入的数值
 * @return void
 */
DREAMSKY_API inline void put_bits_no_assert(PutBitContext* s, int n, BitBuf value)
{
    BitBuf bit_buf;
    int bit_left;

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;

    /* XXX: optimize */
#ifdef BITSTREAM_WRITER_LE
    bit_buf |= value << (BUF_BITS - bit_left);
    if (n >= bit_left) 
    {
        if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) 
        {
            DREAM_WLBUF(s->buf_ptr, bit_buf);
            s->buf_ptr += sizeof(BitBuf);
        }
        else 
        {
            //LOG(NULL, LOG_ERROR, "Internal error, put_bits buffer too small\n");
            assert(0);
        }
        bit_buf = value >> bit_left;
        bit_left += BUF_BITS;
    }
    bit_left -= n;
#else
    if (n < bit_left) 
    {
        bit_buf = (bit_buf << n) | value;
        bit_left -= n;
    }
    else 
    {
        bit_buf <<= bit_left;
        bit_buf |= value >> (n - bit_left);
        if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) 
        {
            DREAM_WBBUF(s->buf_ptr, bit_buf);
            s->buf_ptr += sizeof(BitBuf);
        }
        else 
        {
            //LOG(NULL, LOG_ERROR, "Internal error, put_bits buffer too small\n");
            assert(0);
        }
        bit_left += BUF_BITS - n;
        bit_buf = value;
    }
#endif

    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}

/**
 * @brief 将数值按照 bit 写入到比特流对象中
 * @details 最多只能写入 31 bits，如果超过这个数目，需要使用 put_bits32
 * @param[in] s      写入的比特流对象指针
 * @param[in] n      写入的bits数目
 * @param[in] value  写入的数值
 * @return void
 */
DREAMSKY_API inline void put_bits(PutBitContext* s, int n, BitBuf value)
{
    assert(n <= 31 && value < (1UL << n));
    put_bits_no_assert(s, n, value);
}

/**
 * @brief 将数值按照 bit 写入到比特流对象中
 * @details 
 * - 小端模式！！
 * - 最多只能写入 31 bits，如果超过这个数目，需要使用 put_bits32
 * @param[in] s      写入的比特流对象指针
 * @param[in] n      写入的bits数目
 * @param[in] value  写入的数值
 * @return void
 */
DREAMSKY_API inline void put_bits_le(PutBitContext* s, int n, BitBuf value)
{
    BitBuf bit_buf;
    int bit_left;

    assert(n <= 31 && value < (1UL << n));

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;

    bit_buf |= value << (BUF_BITS - bit_left);
    if (n >= bit_left) 
    {
        if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) 
        {
            DREAM_WLBUF(s->buf_ptr, bit_buf);
            s->buf_ptr += sizeof(BitBuf);
        }
        else 
        {
            //LOG(NULL, LOG_ERROR, "Internal error, put_bits buffer too small\n");
            assert(0);
        }
        bit_buf = value >> bit_left;
        bit_left += BUF_BITS;
    }
    bit_left -= n;

    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}

/**
 * @brief 将数值按照 bit 写入到比特流对象中
 * @details
 * - 数值为有符号整数
 * - 最多只能写入 31 bits
 * @param[in] s      写入的比特流对象指针
 * @param[in] n      写入的bits数目
 * @param[in] value  写入的数值
 * @return void
 */
DREAMSKY_API inline void put_sbits(PutBitContext* pb, int n, int32_t value)
{
    assert(n >= 0 && n <= 31);

    put_bits(pb, n, dreamsky_mod_uintp2(value, n));
}

/**
 * @brief 将数值按照 bit 写入到比特流对象中
 * @details 
 * - 完整的以 32 bits 写入
 * @param[in] s      写入的比特流对象指针
 * @param[in] value  写入的数值
 * @return void
 */
DREAMSKY_API void DREAMSKY_UNUSED put_bits32(PutBitContext* s, uint32_t value)
{
    BitBuf bit_buf;
    int bit_left;

    if (BUF_BITS > 32) 
    {
        put_bits_no_assert(s, 32, value);
        return;
    }

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;

#ifdef BITSTREAM_WRITER_LE
    bit_buf |= (BitBuf)value << (BUF_BITS - bit_left);
    if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) 
    {
        DREAM_WLBUF(s->buf_ptr, bit_buf);
        s->buf_ptr += sizeof(BitBuf);
    }
    else 
    {
        //LOG(NULL, LOG_ERROR, "Internal error, put_bits buffer too small\n");
        assert(0);
    }
    bit_buf = (uint64_t)value >> bit_left;
#else
    bit_buf = (uint64_t)bit_buf << bit_left;
    bit_buf |= (BitBuf)value >> (BUF_BITS - bit_left);
    if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) 
    {
        DREAM_WBBUF(s->buf_ptr, bit_buf);
        s->buf_ptr += sizeof(BitBuf);
    }
    else 
    {
        //LOG(NULL, LOG_ERROR, "Internal error, put_bits buffer too small\n");
        assert(0);
    }
    bit_buf = value;
#endif

    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}

/**
 * @brief 将数值按照 bit 写入到比特流对象中
 * @details 最多可写入 64 bits
 * @param[in] s      写入的比特流对象指针
 * @param[in] n      写入的bits数目
 * @param[in] value  写入的数值
 * @return void
 */
DREAMSKY_API inline void put_bits64(PutBitContext* s, int n, uint64_t value)
{
    assert((n == 64) || (n < 64 && value < (UINT64_C(1) << n)));

    if (n < 32)
        put_bits(s, n, value);
    else if (n == 32)
        put_bits32(s, value);
    else if (n < 64) 
    {
        uint32_t lo = value & 0xffffffff;
        uint32_t hi = value >> 32;
#ifdef BITSTREAM_WRITER_LE
        put_bits32(s, lo);
        put_bits(s, n - 32, hi);
#else
        put_bits(s, n - 32, hi);
        put_bits32(s, lo);
#endif
    }
    else 
    {
        uint32_t lo = value & 0xffffffff;
        uint32_t hi = value >> 32;
#ifdef BITSTREAM_WRITER_LE
        put_bits32(s, lo);
        put_bits32(s, hi);
#else
        put_bits32(s, hi);
        put_bits32(s, lo);
#endif

    }
}

/**
 * @brief 获得指向比特流写入器将放置下一位的字节的指针。
 * @param[in] s      写入的比特流对象指针
 * @return 将放置下一位的字节的指针。
 */
DREAMSKY_API inline uint8_t* put_bits_ptr(PutBitContext* s)
{
    return s->buf_ptr;
}

/**
 * @brief 在比特流对象中跳过给定数目的字节
 * @details 在调用它之前，PutBitContext 必须刷新并对齐到字节边界。
 * @param[in] s      写入的比特流对象指针
 * @param[in] n      需要跳过的字节数目
 * @return void
 */
DREAMSKY_API inline void skip_put_bytes(PutBitContext* s, int n)
{
    assert((put_bits_count(s) & 7) == 0);
    assert(s->bit_left == BUF_BITS);
    assert(n <= s->buf_end - s->buf_ptr);
    s->buf_ptr += n;
}

/**
 * @brief 在比特流对象中跳过给定数目的 bits 
 * @details
 * - 跳过 bit是数目为负数的行为未定义
 * - 只有在比特流中的实际值无关紧要时才必须使用。
 * @param[in] s      写入的比特流对象指针
 * @param[in] n      需要跳过的 bits 数目
 * @return void
 */
DREAMSKY_API inline void skip_put_bits(PutBitContext* s, int n)
{
    unsigned bits = BUF_BITS - s->bit_left + n;
    s->buf_ptr += sizeof(BitBuf) * (bits / BUF_BITS);
    s->bit_left = BUF_BITS - (bits & (BUF_BITS - 1));
}

/**
 * Change the end of the buffer.
 * @brief 更改 buf 的结束位置
 * @param[in] s      写入的比特流对象指针
 * @param[in] size   新的结束位置(以字节)
 * @return void
 */
DREAMSKY_API inline void set_put_bits_buffer_size(PutBitContext* s, int size)
{
    assert(size <= INT_MAX / 8 - BUF_BITS);
    s->buf_end = s->buf + size;
}

/**
 * @brief 用零填充比特流直到下一个字节边界。
 * @param[in] s      写入的比特流对象指针
 * @return void
 */
DREAMSKY_API inline void align_put_bits(PutBitContext* s)
{
    put_bits(s, s->bit_left & 7, 0);
}

#undef DREAM_WBBUF
#undef DREAM_WLBUF

#endif //!__PUT_BITS_H__