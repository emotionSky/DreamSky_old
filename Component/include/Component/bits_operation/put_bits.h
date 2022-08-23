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

//һ��buf�ж��ٸ�bits
static const int BUF_BITS = 8 * sizeof(BitBuf);

/**@struct PutBitContext
 * @brief ��bitд��buf�������Ķ���
 */
struct PutBitContext 
{
	BitBuf bit_buf;
	int bit_left;
	uint8_t* buf, * buf_ptr, * buf_end;
};

/**
 * @brief ��ʼ�� PutBitContext ����
 * @param[in] s            д��ı���������ָ��
 * @param[in] buffer       ��Ҫд�� bit ��bufָ��
 * @param[in] buffer_size  buf �Ĵ�С(�ֽ���)
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
 * @brief ��ȡ�Ѿ�д������������� bits ��Ŀ
 * @param[in] s    д��ı���������ָ��
 * @return д��� bits ��Ŀ
 */
DREAMSKY_API inline int put_bits_count(PutBitContext* s)
{
    return (s->buf_ptr - s->buf) * 8 + BUF_BITS - s->bit_left;
}

/**
 * @brief ��ȡĿǰΪֹ������ֽ���Ŀ
 * @detail ֻ�����³�ʼ����ˢ�� PutBitContext ʱ���á�
 * @param[in] s    д��ı���������ָ��
 * @return ������ֽ���Ŀ
 */
DREAMSKY_API inline int put_bytes_output(const PutBitContext* s)
{
    assert(s->bit_left == BUF_BITS);
    return s->buf_ptr - s->buf;
}

/**
 * @brief ��ȡĿǰΪֹ������ֽ���Ŀ
 * @detail ��������ʱ��ʹ�� 
 * @param[in] s         д��ı���������ָ��
 * @param[in] round_up  ���ú󣬵�ĿǰΪֹд���λ�����������뵽��һ���ֽڡ�
 * @return  ĿǰΪֹ������ֽ���Ŀ
 */
DREAMSKY_API inline int put_bytes_count(const PutBitContext* s, int round_up)
{
    return s->buf_ptr - s->buf + ((BUF_BITS - s->bit_left + (round_up ? 7 : 0)) >> 3);
}

/**
 * @brief ���������������õ��µ� buf
 * @param[in] s            д��ı���������ָ��
 * @param[in] buffer       ��Ҫд�� bit ��bufָ��
 * @param[in] buffer_size  buf �Ĵ�С(�ֽ���)
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
 * @brief ��ȡ�����������л�ʣ��� bits ��Ŀ
 * @param[in] s         д��ı���������ָ��
 * @return  ��ʹ�õ� bits ��Ŀ
 */
DREAMSKY_API inline int put_bits_left(PutBitContext* s)
{
    return (s->buf_end - s->buf_ptr) * 8 - BUF_BITS + s->bit_left;
}

/**
 * @brief ��ȡʣ����ֽ���Ŀ
 * @param[in] s         д��ı���������ָ��
 * @param[in] round_up  ���ú�д���λ�����������뵽��һ���ֽڡ�
 * @return ʣ���ֽ���Ŀ
 */
DREAMSKY_API inline int put_bytes_left(const PutBitContext* s, int round_up)
{
    return s->buf_end - s->buf_ptr - ((BUF_BITS - s->bit_left + (round_up ? 7 : 0)) >> 3);
}

/**
 * @brief flush ����������
 * @detail �� 0 ��仹δ����Ĳ���
 * @param[in] s         д��ı���������ָ��
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
 * @brief flush ����������
 * @detail �� 0 ��仹δ����Ĳ���
 * @param[in] s         д��ı���������ָ��
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
 * @brief ���ַ������뵽������������
 * @param[in] pb                д��ı���������ָ��
 * @param[in] string            ��Ҫд����ַ���
 * @param[in] terminate_string  �������Ϊ1����ô�������ַ���д��֮���Զ���һ���ַ���������
 * @return void
 */
void dreamsky_put_string(PutBitContext* pb, const char* string, int terminate_string);

/**
 * @brief ��һ�� buf �е����ݿ�����������������
 * @param[in] pb      д��ı���������ָ��
 * @param[in] src     ��Ҫ������ buf
 * @param[in] length  ������ bits ��Ŀ
 * @return void
 */
void dreamsky_copy_bits(PutBitContext* pb, const uint8_t* src, int length);
#endif

/**
 * @brief ����ֵ���� bit д�뵽������������
 * @details û���쳣�׳�
 * @param[in] s      д��ı���������ָ��
 * @param[in] n      д���bits��Ŀ
 * @param[in] value  д�����ֵ
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
 * @brief ����ֵ���� bit д�뵽������������
 * @details ���ֻ��д�� 31 bits��������������Ŀ����Ҫʹ�� put_bits32
 * @param[in] s      д��ı���������ָ��
 * @param[in] n      д���bits��Ŀ
 * @param[in] value  д�����ֵ
 * @return void
 */
DREAMSKY_API inline void put_bits(PutBitContext* s, int n, BitBuf value)
{
    assert(n <= 31 && value < (1UL << n));
    put_bits_no_assert(s, n, value);
}

/**
 * @brief ����ֵ���� bit д�뵽������������
 * @details 
 * - С��ģʽ����
 * - ���ֻ��д�� 31 bits��������������Ŀ����Ҫʹ�� put_bits32
 * @param[in] s      д��ı���������ָ��
 * @param[in] n      д���bits��Ŀ
 * @param[in] value  д�����ֵ
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
 * @brief ����ֵ���� bit д�뵽������������
 * @details
 * - ��ֵΪ�з�������
 * - ���ֻ��д�� 31 bits
 * @param[in] s      д��ı���������ָ��
 * @param[in] n      д���bits��Ŀ
 * @param[in] value  д�����ֵ
 * @return void
 */
DREAMSKY_API inline void put_sbits(PutBitContext* pb, int n, int32_t value)
{
    assert(n >= 0 && n <= 31);

    put_bits(pb, n, dreamsky_mod_uintp2(value, n));
}

/**
 * @brief ����ֵ���� bit д�뵽������������
 * @details 
 * - �������� 32 bits д��
 * @param[in] s      д��ı���������ָ��
 * @param[in] value  д�����ֵ
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
 * @brief ����ֵ���� bit д�뵽������������
 * @details ����д�� 64 bits
 * @param[in] s      д��ı���������ָ��
 * @param[in] n      д���bits��Ŀ
 * @param[in] value  д�����ֵ
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
 * @brief ���ָ�������д������������һλ���ֽڵ�ָ�롣
 * @param[in] s      д��ı���������ָ��
 * @return ��������һλ���ֽڵ�ָ�롣
 */
DREAMSKY_API inline uint8_t* put_bits_ptr(PutBitContext* s)
{
    return s->buf_ptr;
}

/**
 * @brief �ڱ���������������������Ŀ���ֽ�
 * @details �ڵ�����֮ǰ��PutBitContext ����ˢ�²����뵽�ֽڱ߽硣
 * @param[in] s      д��ı���������ָ��
 * @param[in] n      ��Ҫ�������ֽ���Ŀ
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
 * @brief �ڱ���������������������Ŀ�� bits 
 * @details
 * - ���� bit����ĿΪ��������Ϊδ����
 * - ֻ���ڱ������е�ʵ��ֵ�޹ؽ�Ҫʱ�ű���ʹ�á�
 * @param[in] s      д��ı���������ָ��
 * @param[in] n      ��Ҫ������ bits ��Ŀ
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
 * @brief ���� buf �Ľ���λ��
 * @param[in] s      д��ı���������ָ��
 * @param[in] size   �µĽ���λ��(���ֽ�)
 * @return void
 */
DREAMSKY_API inline void set_put_bits_buffer_size(PutBitContext* s, int size)
{
    assert(size <= INT_MAX / 8 - BUF_BITS);
    s->buf_end = s->buf + size;
}

/**
 * @brief ������������ֱ����һ���ֽڱ߽硣
 * @param[in] s      д��ı���������ָ��
 * @return void
 */
DREAMSKY_API inline void align_put_bits(PutBitContext* s)
{
    put_bits(s, s->bit_left & 7, 0);
}

#undef DREAM_WBBUF
#undef DREAM_WLBUF

#endif //!__PUT_BITS_H__