#ifndef __DREAM_MATH_H__
#define __DREAM_MATH_H__

#include <Component/dream_component.h>

/**
 * 比较器
 * 对于两个数值表达式： x y
 * 如果 x>y :  res = 1
 * 如果 x<y :  res = -1
 * 如果 x=y :  res = 0
 * 场景：例如，这在 qsort 比较器回调中很有用。
 * 此外，编译器能够将其优化为无分支代码，并且没有符号类型溢出的风险。
 * 与许多宏一样，这会多次评估其参数，因此它不能有副作用。
 */
#define DREAM_DIFF_SIGN(x,y) (((x)>(y)) - ((x)<(y)))

/**
 * 两个数值取较大
 */
#define DREAM_MAX(a,b) ((a) > (b) ? (a) : (b))

/**
* 三个数值取较大
*/
#define DREAM_MAX3(a,b,c) FFMAX(FFMAX(a,b),c)

/**
* 两个数值取较小
*/
#define DREAM_MIN(a,b) ((a) > (b) ? (b) : (a))

/**
* 三个数值取较小
*/
#define DREAM_MIN3(a,b,c) DREAM_MIN(DREAM_MIN(a,b),c)

/**
* 根据类型进行两个数值的交换
*/
#define DREAM_SWAP(type,a,b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)

/**
* 获取一个数组的长度
*/
#define DREAM_ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

/**
 * 大端模式下的 
 */
#define MKTAG(a,b,c,d)   ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))

/**
* 小端模式下的
*/
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))


/**
 * @brief 从特定位位置开始的无符号整数中清除高位
 * @param[in]  a 要清除高位的值
 * @param[in]  p 要清除的bit位置
 * @return 清除后的数值
 */
static DREAMSKY_ALWAYS_INLINE DREAMSKY_CONST unsigned dreamsky_mod_uintp2_c(unsigned a, unsigned p)
{
    return a & ((1U << p) - 1);
}

#ifndef dreamsky_mod_uintp2
    #define dreamsky_mod_uintp2    dreamsky_mod_uintp2_c
#endif

#endif //!__DREAM_MATH_H__