#ifndef __LOG_MACRO_H__
#define __LOG_MACRO_H__

#define u_char unsigned char ///<仅仅是为了减少写的字符数量
#define u_int  unsigned int  ///<仅仅是为了减少写的字符数量
#define LOG_MAX_STR_NUM 2048 ///<打印的日志信息的最大字符数量
#define PATH_LEN        512  ///<路径的最大字符长度

#define LOG_MAX_UINT32_VALUE   (uint32_t) 0xffffffff                ///<最大的32位无符号数：十进制是‭4294967295‬
#define LOG_INT64_LEN          (sizeof("-9223372036854775808") - 1)     

/** 宏定义函数 */
/** 类似memcpy，但常规memcpy返回的是指向目标dst的指针，而这个ngx_cpymem返回的是目标【拷贝数据后】的终点位置，连续复制多段数据时方便 */
#define log_memcpy(dst, src, n)   (((u_char *) memcpy(dst, src, n)) + (n))  ///<注意#define写法，n这里用()包着，防止出现什么错误
#define log_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))              ///<比较大小，返回小值，注意，参数都用()包着   

#endif // !__LOG_MACRO_H__