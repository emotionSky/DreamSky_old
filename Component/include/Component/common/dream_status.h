/**
 * \file dream_status.h
 * \date 2023/05/09 09:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 一些状态使用
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 参考的开源项目
 */

#ifndef __DREAM_STATUS_H__
#define __DREAM_STATUS_H__

#include "dream_define.h"

DREAMSKY_NAMESPACE_BEGIN

enum DStatus
{
    DREAM_BAD_PARAMS = -2,
    DREAM_FAILED  = -1,
    DREAM_SUCCESS = 0,
};

DREAMSKY_NAMESPACE_END

#endif //__DREAM_STATUS_H__
