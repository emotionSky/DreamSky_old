
#ifndef __DREAM_PTR_H__
#define __DREAM_PTR_H__

#include <memory>

/**
 * \brief 兼容std::make_unique的语法
 * \details 实际上这里的作用相当于： std::make_unique<T>(args)
 * 
 * \param args 不定参数
 * \return std::unique_ptr<T> 
 */
template<typename T, typename... Args>
typename std::unique_ptr<T> d_make_unique(Args&&... args)
{
	// forward 的用法可参考：
	// https://zhuanlan.zhihu.com/p/92486757
	return std::unique_ptr<T>(new T(std::forward(args)...));
}

//微软的源码实现
//template <class _Ty, class... _Types, enable_if_t<!is_array_v<_Ty>, int> = 0>
//_NODISCARD unique_ptr<_Ty> make_unique(_Types&&... _Args) { // make a unique_ptr
//	return unique_ptr<_Ty>(new _Ty(_STD forward<_Types>(_Args)...));
//}

#endif //!__DREAM_PTR_H__