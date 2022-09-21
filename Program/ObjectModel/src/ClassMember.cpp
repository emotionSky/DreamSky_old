#include "ClassMember.h"
#include <cstdio>

ClassMember::ClassMember() :
	m_i(0), m_j(0), m_c('c'), m_k(0), m_f(1.1f)
{

}

ClassMember::~ClassMember()
{
}

void ClassMember::PrintMemberDistance()
{
	printf("----------成员变量地址偏移值----------\n");
	printf("ClassMember::m_i = %d.\n", &ClassMember::m_i);
	printf("ClassMember::m_j = %d.\n", &ClassMember::m_j);
	printf("ClassMember::m_c = %d.\n", &ClassMember::m_c);
	printf("ClassMember::m_k = %d.\n", &ClassMember::m_k);
	printf("ClassMember::m_f = %d.\n", &ClassMember::m_f);
	printf("\n");
}

void ClassMember::PrintMemberAddress()
{
	printf("----------成员变量地址数值----------\n");
	printf("ClassMember::m_i = %p.\n", &m_i);
	printf("ClassMember::m_j = %p.\n", &m_j);
	printf("ClassMember::m_c = %p.\n", &m_c);
	printf("ClassMember::m_k = %p.\n", &m_k);
	printf("ClassMember::m_f = %p.\n", &m_f);
	printf("\n");
}

void ClassMember::PrintInfo()
{
	printf("ClassMember::PrintInfo printf.\n");
}

void ClassMember::PrintNum(int num)
{
	printf("ClassMember::PrintNum num=%d.\n", num);
}

void ClassMember::PointerFuncString(const char* str)
{
	printf("ClassMember::PointerFuncString str=%s.\n", str);
}

void ClassMember::PointerFuncNum(int num)
{
	printf("ClassMember::PointerFuncNum num=%d.\n", num);
}

void ClassMember::PointerFuncStatic(float f)
{
	printf("ClassMember::PointerFuncStatic f=%f.\n", f);
}

void ClassMemberTest()
{
	ClassMember t;
	printf("sizeof(ClassMember)=%lld.\n", sizeof(t));
	t.PrintMemberDistance();
	t.PrintMemberAddress();
}

template<typename dst_type, typename src_type>
inline dst_type pointer_cast(src_type src)
{
	return *static_cast<dst_type*>(static_cast<void*>(&src));
}

void ClassFunctionCast()
{
	printf("ClassMember::PrintInfo的地址为:%p.\n", &ClassMember::PrintInfo);
	using Func = void(*)(void);
	//typedef void(*Func)();
	Func func = pointer_cast<Func>(&ClassMember::PrintInfo);
	//void* func2 = pointer_cast<void*>(&ClassMember::PrintInfo);
	func();

	/*
	printf("ClassMember::PrintNum的地址为:%p.\n", &ClassMember::PrintNum);
	using NumFunc = void(*)(int);
	NumFunc nfunc = pointer_cast<NumFunc>(&ClassMember::PrintNum);
	nfunc(12); //输出并不一定是12，并且不一定能够正常运行
	*/
}

void ClassFunctionPointer()
{
	const char* str = "test ClassFunctionPointer";
	ClassMember cm;
	using func_static = void(*)(float);
	using func_int = void(ClassMember::*)(int);
	using func_str = void(ClassMember::*)(const char*);
	//typedef void(ClassMember::* func_str)(const char*);

	func_static func0 = &ClassMember::PointerFuncStatic;
	func_int func1 = &ClassMember::PointerFuncNum;
	func_str func2 = &ClassMember::PointerFuncString;

	func0(12.34f);
	(cm.*func1)(11);
	(cm.*func2)(str);
}