#ifndef __LOG_H__
#define __LOG_H__

#define LOG_STDERR            0    ///<控制台错误【stderr】：最高级别日志，日志的内容不再写入log参数指定的文件，而是会直接将日志输出到标准错误设备比如控制台屏幕
#define LOG_EMERG             1    ///<紧急 【emerg】
#define LOG_ALERT             2    ///<警戒 【alert】
#define LOG_CRIT              3    ///<严重 【crit】
#define LOG_ERROR             4    ///<错误 【error】：属于常用级别
#define LOG_WARN              5    ///<警告 【warn】：属于常用级别
#define LOG_NOTICE            6    ///<注意 【notice】
#define LOG_INFO              7    ///<信息 【info】
#define LOG_DEBUG             8    ///<调试 【debug】：最低级别

#define ERROR_LOG_PATH       "log"   ///<定义日志的默认存放的文件夹，文件名称需要按照时间或者大小进行处理

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @brief log初始化的函数
 * @param[in] path       log文件存放的路径
 * @param[in] log_level  日志的限制级别，对应上述的宏定义
 */
void log_init(const char* path, int log_level);

/**
 * @brief 打印标准错误的函数
 * @details 通过可变参数组合出字符串【支持...省略号形参】，\n
 *          自动往字符串最末尾增加换行符【所以调用者不用加\n】，\n
 *          往标准错误上输出这个字符；如果err不为0，表示有错误，\n
 *          会将该错误编号以及对应的错误信息一并放到组合出的字符串中一起显示。
 * @param[in] err   对应的标准错误编号
 * @param[in] fmt   字符串的格式化
 * @param[in] ...   可变参数
 * @return void
 * @example \n
 * - log_stderr(0, "invalid option: \"%s\"", "nginx");          //invalid option: "nginx" \n
 * - log_stderr(0, "invalid option: %10d", 21);                 //invalid option:         21  ---21前面有8个空格 \n
 * - log_stderr(0, "invalid option: %.6f", 21.378);             //invalid option: 21.378000   ---%.这种只跟f配合有效，往末尾填充0 \n
 * - log_stderr(0, "invalid option: %.6f", 12.999);             //invalid option: 12.999000 \n
 * - log_stderr(0, "invalid option: %.2f", 12.999);             //invalid option: 13.00 \n
 * - log_stderr(0, "invalid option: %d", 1678);                 //invalid option: 1678 \n
 * - log_stderr(0, "invalid option: %xd", 1678);                //invalid option: 68e \n
 * - log_stderr(0, "invalid option: %Xd", 1678);                //invalid option: 68E \n
 * - log_stderr(15, "invalid option: %s , %d", "testInfo",326); //invalid option: testInfo , 326
 */ 
void log_stderr(int err, const char *fmt, ...);

/**
* @brief 打印日志的核心函数
* @details 代码中有自动加换行符，所以调用时字符串不用刻意加\n；\n
*          如果定向为标准错误，则直接往屏幕上写日志。
* @param[in]  level  日志的等级
* @param[in]  err    标准错误码，当这个错误码不为零的时候，需要自动将标准错误信息写入到日志文件中
* @param[in]  fmt    日志字符串的格式
* @param[in]  ...    可变参数
* @return void
*/  
void log_error_core(int level, int err, const char *fmt, ...);

/**
 * @brief 释放log文件
 * @return void
 */
void log_release();

#ifdef  __cplusplus
}
#endif

#endif // !__LOG_H__