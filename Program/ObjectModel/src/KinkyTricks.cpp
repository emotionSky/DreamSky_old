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
	 * �����ͨ�������麯������ʵ�ֵ�
	 * ��64λϵͳ��˵��
	 * ��Ϊ���࣬��ôֻ���麯����ָ����һ��������������sizeof(KinkyTricks) = 8��
	 * ��ô���ʱ��Ϳ����õ�����ĵ�ַ��long long* ptr = (long long*)&kt ��
	 * Ȼ������õ��麮��ָ�� long long* vptr = (long long*)(* ptr);
	 * �ٶ���һ������ָ������ using Func = void(*)(void);
	 * ��ô�Ϳ��Խ��и�ֵ�� Func func = (Func)vptr[0];
	 * Ȼ����ú��� func();
	 */
}