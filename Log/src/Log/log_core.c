#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/timeb.h>

#ifdef WIN32
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#else
#include <unistd.h>
#endif

#include "Log/log_core.h"
#include "log_macro.h"
#include "log_printf.h"

/**
 * @struct log_t
 * @brief  日志相关参数的结构体
 */
typedef struct
{
    char path[PATH_LEN]; ///<日志路径
    int log_level;       ///<日志级别，对应上述宏定义的几个日志级别
    int fd;              ///<日志文件描述符
} log_t;

log_t log_obj; ///<实例化，保证变量的唯一

/**
 * @brief 获取文件的名称，需要根据系统时间进行配置
 * @param[out] name  文件名称的字符串
 */
static void get_file_name(char *name);

/**
 * @brief 将根据错误编号组合的一个字符串复制到一段buf中
 * @details 形如：  (错误编号：错误原因)
 * @param[in] buf   buf的首地址
 * @param[in] last  buf的尾地址，不能超过这个位置
 * @param[in] err   错误码
 * @return u_char* 写入完成之后的buf的首地址
 */
static u_char *log_errno(u_char *buf, u_char *last, int err);

/** 错误等级和对应的字符串的映射，使用数组的方式实现 */
static u_char err_levels[][20] =
    {
        {"stderr"}, ///< 0：控制台错误
        {"emerg"},  ///< 1：紧急
        {"alert"},  ///< 2：警戒
        {"crit"},   ///< 3：严重
        {"error"},  ///< 4：错误
        {"warn"},   ///< 5：警告
        {"notice"}, ///< 6：注意
        {"info"},   ///< 7：信息
        {"debug"}   ///< 8：调试
};

void log_init(const char *path, int log_level)
{
    //只写打开|追加到末尾|文件不存在则创建【这个需要跟第三参数指定文件访问权限】
    // mode = 0644：文件访问权限， 6: 110    , 4: 100：     【用户：读写， 用户所在组：读，其他：读】 老师在第三章第一节介绍过
    // ngx_log.fd = open((const char *)plogname,O_WRONLY|O_APPEND|O_CREAT|O_DIRECT,0644);   //绕过内和缓冲区，write()成功则写磁盘必然成功，但效率可能会比较低；

    memset(log_obj.path, 0, PATH_LEN);
    if (path)
    {
        strcpy(log_obj.path, path);
    }
    else
    {
        sprintf(log_obj.path, "./%s", ERROR_LOG_PATH);
    }

    char log_path[PATH_LEN] = {0};
    char name[40] = {0};
    get_file_name(name);
    sprintf(log_path, "%s/%s", log_obj.path, name);
    log_obj.log_level = log_level;

    /**
     * 只写打开|追加到末尾|文件不存在则创建【这个需要跟第三参数指定文件访问权限】
     * mode = 0644：文件访问权限， 6: 110 , 4: 100：【用户：读写， 用户所在组：读，其他：读】
     */
    log_obj.fd = open(log_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (log_obj.fd == -1)
    {
        /** 如果有错误，则直接定位到 标准错误上去  */
        log_stderr(errno, "[alert] could not open error log file: %s", log_path);
        log_obj.fd = STDERR_FILENO; //直接定位到标准错误去了
    }
    return;
}

void log_stderr(int err, const char *fmt, ...)
{
    va_list args;                       ///<创建一个va_list类型变量
    u_char errstr[LOG_MAX_STR_NUM + 1]; ///<这里进行+1保证字符串的最后一个字符是'\0'，保证安全
    memset(errstr, 0, sizeof(errstr));  ///<初始化保证安全
    u_char *p, *last;

    /**
     * last指向了errstr[LOG_MAX_STR_NUM + 1]的最后一个位置，确保不会使用超出边界的内存。\n
     * 比如你定义 u_char tmp[2]; 你如果last = tmp+2，那么last实际指向了tmp[2]，而tmp[2]在使用中是无效的。
     */
    last = errstr + LOG_MAX_STR_NUM;
    p = errstr;

    va_start(args, fmt);                   ///<使args指向起始的参数
#if USE_STD_VSNPRINTF
    int ret = vsnprintf(p, (last - p), fmt, args); ///<使用vsnprintf函数
    if (ret < 0)
    {
        va_end(args);                     ///<释放args
        return;
    }
    p += ret;
#else
    p = log_vslprintf(p, last, fmt, args); ///<组合出这个字符串保存在errstr里
#endif
    va_end(args);                          ///<释放args

    ///<注意的是这里如果err不为零，表示有标准错误发生，那么就需要进行打印
    if (err)
    {
        p = log_errno(p, last, err);
    }

    /**
     * 这里需要考虑的是，即使给的buf并不能完全放下所有的日志字符串信息，
     * 那么换行符也要强行插入到末尾的位置，覆盖其他的内容也在所不惜。
     */
    if (p >= (last - 1))
    {
        p = (last - 1) - 1; ///<这里就是通过处理，将 p 指向倒数第二个位置，从而能够保证最后的字符为"\n\0"。
    }
    *p++ = '\n'; ///<强行替换为换行符'\n'

    ///<向标准错误【一般是屏幕】输出信息，这里的 STDERR_FILENO 就是标准错误文件描述符，一般指屏幕
    write(STDERR_FILENO, errstr, p - errstr);

    if (log_obj.fd > STDERR_FILENO) ///<如果这是个有效的日志文件，本条件肯定成立，此时也才有意义将这个信息写到日志文件
    {
        ///<因为上边已经把err信息显示出来了，所以这里就不要显示了，否则显示重复了
        err = 0; ///<不要再次把错误信息弄到字符串里，否则字符串里重复了
        p--;
        *p = 0; ///<把原来末尾的\n干掉，因为到ngx_log_err_core中还会加这个\n
        log_error_core(LOG_STDERR, err, (const char *)errstr);
    }
    return;
}

u_char *log_errno(u_char *buf, u_char *last, int err)
{
    char *perrorinfo = strerror(err); ///<根据资料不会返回NULL
    size_t len = strlen(perrorinfo);  ///<获取错误信息的长度

    char leftstr[10] = {0};
    sprintf(leftstr, " (%d: ", err);
    size_t leftlen = strlen(leftstr);

    char rightstr[] = ") ";
    size_t rightlen = strlen(rightstr);

    size_t extralen = leftlen + rightlen; ///<左右的额外宽度
    if ((buf + len + extralen) < last)
    {
        ///<保证整个我装得下，我就装，否则我全部抛弃 ,nginx的做法是 如果位置不够，就硬留出50个位置【哪怕覆盖掉以往的有效内容】，也要硬往后边塞，这样当然也可以；
        buf = log_memcpy(buf, leftstr, leftlen);
        buf = log_memcpy(buf, perrorinfo, len);
        buf = log_memcpy(buf, rightstr, rightlen);
    }
    return buf;
}

void log_error_core(int level, int err, const char *fmt, ...)
{
    u_char *last, *p;                   ///< buf的尾地址，指向当前要拷贝数据到其中的内存位置
    u_char errstr[LOG_MAX_STR_NUM + 1]; ///<这里进行+1保证字符串的最后一个字符是'\0'，保证安全

    memset(errstr, 0, sizeof(errstr));
    last = errstr + LOG_MAX_STR_NUM;

    struct timeb fb;
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    va_list args;

    ftime(&fb);
#ifdef WIN32
    localtime_s(&tm, &fb.time);
#else
    localtime_r(&fb.time, &tm);
#endif

    tm.tm_mon++;
    tm.tm_year += 1900;

    u_char strcurrtime[40] = {0}; ///<先组合出一个当前时间字符串，格式形如：2019/01/08 19:57:11.123
    log_slprintf(strcurrtime,
                 (u_char *)-1, ///<若用一个u_char *接一个 (u_char *)-1,则 得到的结果是 0xffffffff....，这个值足够大
                 "%4d/%02d/%02d %02d:%02d:%02d.%03d",
                 tm.tm_year, tm.tm_mon,
                 tm.tm_mday, tm.tm_hour,
                 tm.tm_min, tm.tm_sec, fb.millitm);
    p = log_memcpy(errstr, strcurrtime, strlen((const char *)strcurrtime)); ///<日期增加进来，得到形如：     2019/01/08 20:26:07
    p = log_slprintf(p, last, " [%s] ", err_levels[level]);                 ///<日志级别增加进来，得到形如：  2019/01/08 20:26:07 [crit]

    va_start(args, fmt);                   ///<使args指向起始的参数
#if USE_STD_VSNPRINTF
	int ret = vsnprintf(p, (last - p), fmt, args); ///<使用vsnprintf函数
	if (ret < 0)
	{
		va_end(args);                     ///<释放args
		return;
	}
	p += ret;
#else
    p = log_vslprintf(p, last, fmt, args); ///<把fmt和args参数弄进去，组合出来这个字符串
#endif
    va_end(args);                          ///<释放args

    if (err)
    {
        ///<如果错误代码不是0，错误代码和错误信息也要显示出来
        p = log_errno(p, last, err);
    }
    if (p >= (last - 1))
    {
        ///<若位置不够，那换行也要硬插入到末尾，哪怕覆盖到其他内容
        p = (last - 1) - 1;
    }
    *p++ = '\n'; //增加个换行符

    int n;
    while (1)
    {
        ///<这么写代码是图方便：随时可以把流程弄到while后边去；大家可以借鉴一下这种写法
        if (level > log_obj.log_level)
        {
            ///<超过日志级别的打印就不需要打印了
            break;
        }

        /** 磁盘是否写满的判断啊什么的，暂时先不做实现 */

        /** 写日志文件 */
        n = write(log_obj.fd, errstr, p - errstr);
        if (n == -1)
        {
            ///<写失败有问题
            if (errno == ENOSPC)
            {
                //磁盘没空间了
                //没空间还写个毛线啊
                //先do nothing吧；
            }
            else
            {
                ///<这是有其他错误，那么我考虑把这个错误显示到标准错误设备吧；
                if (log_obj.fd != STDERR_FILENO) ///<当前是定位到文件的，则条件成立
                {
                    n = write(STDERR_FILENO, errstr, p - errstr);
                }
            }
        }
        break;
    } // end while
    return;
}

void log_release()
{
    if (log_obj.fd != STDERR_FILENO && log_obj.fd != -1)
    {
        close(log_obj.fd);
        log_obj.fd = -1;
    }
}

static void get_file_name(char *name)
{
    time_t now = time(NULL);
    struct tm dt;
#ifdef WIN32
    localtime_s(&dt, &now);
#else
    localtime_r(&now, &dt);
#endif
    sprintf(name, "%04d%02d%02d.log", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
}