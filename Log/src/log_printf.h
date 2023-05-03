#ifndef __LOG_PRINTF_H__
#define __LOG_PRINTF_H__

#include <stdarg.h>
#include "log_macro.h"

/**
 * @brief 自定义的标准格式化输出
 * @details 最终还是通过调用log_vslprintf来实现具体的功能
 * @param[in] buf   需要存放数据的buf的起始位置
 * @param[in] last  存放数据的buf的结束位置，不可超过这个位置
 * @param[in] fmt   字符串的格式化 
 * @param[in] ...   可变参数
 * @return u_char* 按照一定格式化后的字符串起始位置
 */
u_char *log_slprintf(u_char *buf, u_char *last, const char *fmt, ...);

/**
 * @brief 打印处理buf的函数
 * @details 此处函数实现类似于 vprintf 。\n
 *          假设传进行的是 "abc=%d", 13 ，那么在buf里得到的应该是 abc=13 这样的结果
 * @param[in] buf   需要存放数据的buf的起始位置
 * @param[in] last  存放数据的buf的结束位置，不可超过这个位置
 * @param[in] fmt   字符串的格式化
 * @param[in] args  一些列的可变参数
 * @return u_char* 按照一定格式化后的字符串起始位置(填入之后，可以继续填入的位置的初始值)
 */
u_char *log_vslprintf(u_char *buf, u_char *last, const char *fmt, va_list args);

#endif // !__LOG_PRINTF_H__