#include "KinkyTricks.h"
#include <cstdio>

KinkyTricks::KinkyTricks()
{
}

KinkyTricks::~KinkyTricks()
{
}

void KinkyTricks::VirtualFunc()
{
	printf("KinkyTricks::VirtualFunc, virtual!!!\n");
}

void VirtualFunctionTest()
{
	KinkyTricks kt;
	//64bits
	(reinterpret_cast<void(*)()>(**(long long**)(&kt)))();
	//32bits
	//(reinterpret_cast<void(*)()>(**(int**)(&kt)))();

	/**
	 * 这就是通过调用虚函数表来实现的
	 * 以64位系统来说：
	 * 类为空类，那么只有虚函数表指针这一个“变量”，即sizeof(KinkyTricks) = 8，
	 * 那么这个时候就可以拿到对象的地址，long long* ptr = (long long*)&kt ；
	 * 然后可以拿到虚寒表指针 long long* vptr = (long long*)(* ptr);
	 * 再定义一个函数指针类型 using Func = void(*)(void);
	 * 那么就可以进行赋值了 Func func = (Func)vptr[0];
	 * 然后调用函数 func();
	 */
}