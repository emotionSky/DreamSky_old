#ifndef __DTIME_H__
#define __DTIME_H__

#include "common_type.h"
#include <cstdint>

DREAMSKY_API void SleepUs(unsigned usec);
DREAMSKY_API void SleepMs(unsigned msec);
DREAMSKY_API uint64_t GetCurrentMs();

#endif //!__DTIME_H__