/**
 * \file dream_object.h
 * \date 2023/05/09 09:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 一个对象基类，作为后续整个框架的基类
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 参考的开源项目
 */

#ifndef __DREAM_OBJECT_H__
#define __DREAM_OBJECT_H__

#include "dream_define.h"
#include "dream_status.h"

DREAMSKY_NAMESPACE_BEGIN

class DObject
{
public:
    // explicit禁用隐式构造
    // =default要求编译器生成默认构造函数，避免我们写的其他构造函数导致默认构造函数没有了
    explicit DObject() = default;

    // 默认析构函数
    virtual ~DObject() = default;

    virtual DStatus Init() = 0;

    virtual void Run() = 0;

    virtual void Destroy() = 0;

private:
};

DREAMSKY_NAMESPACE_END

#endif //__DREAM_OBJECT_H__
