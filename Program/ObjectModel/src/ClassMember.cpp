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

void ClassMemberTest()
{
	ClassMember t;
	printf("sizeof(ClassMember)=%lld.\n", sizeof(t));
	t.PrintMemberDistance();
	t.PrintMemberAddress();
}