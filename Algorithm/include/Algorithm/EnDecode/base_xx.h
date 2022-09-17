#ifndef __BASE_XX_H__
#define __BASE_XX_H__

#include "Algorithm/common_type.h"

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif

#define BASE_XX_ERROR   -1
#define BASE_XX_SUCCESS 0

DREAMSKY_API int base16_encode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len);

DREAMSKY_API int base16_decode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len);

DREAMSKY_API int base32_encode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len);

DREAMSKY_API int base32_decode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len);

DREAMSKY_API int base64_encode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len);

DREAMSKY_API int base64_decode(const uint8_t* src, int src_len, uint8_t* dst, int* dst_len);

#ifdef __cplusplus
}
#endif

#endif //!__BASE_XX_H__