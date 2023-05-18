/**
 * \file dream_exception.h
 * \date 2023/05/05 21:08
 *
 * \author 凌佚
 * Contact: bitdreamsky@126.com
 *
 * \brief 简单的示例，如何继承标准异常，实现一个自定的异常类
 *
 * TODO: 逐步进行完善，根据阅读的越来越多的开源库进行完善，
 *       每一次阅读新的开源库都会有新的体会。
 *       那么，都需要将其化为己用！
 *
 * \note 参考的开源项目
 * CThreadPool: https://github.com/ChunelFeng/CThreadPool.git
 */

#ifndef __DREAM_EXCEPTION_H__
#define __DREAM_EXCEPTION_H__

#include "dream_define.h"
#include <exception>
#include <string>

DREAMSKY_NAMESPACE_BEGIN

constexpr auto DREAM_DEFAULT_EXCEPTION = "default exception";

/**
 * \class DException
 *
 * \brief 继承标准异常的自用异常
 */
class DException : public std::exception
{
public:
	// 使用 explicit 去禁用隐式构造
	explicit DException(const std::string& info) : 
		m_info(info.empty() ? DREAM_DEFAULT_EXCEPTION : info)
	{}

	/**
	 * \brief  获取异常信息字符串
	 * \return 异常信息的字符串指针
	 */
	[[nodiscard]] const char* what() const noexcept override
    {
		return m_info.c_str();
	}
	// 使用 [[nodiscard]] 来要求调用的地方必须使用返回值

private:
	std::string m_info;
};

DREAMSKY_NAMESPACE_END

#endif //!__DREAM_EXCEPTION_H__