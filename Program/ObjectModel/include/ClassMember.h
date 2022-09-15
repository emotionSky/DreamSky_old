#ifndef __CLASS_MEMBER_H__
#define __CLASS_MEMBER_H__

class ClassMember
{
public:
	ClassMember();
	~ClassMember();

	void PrintMemberDistance();
	void PrintMemberAddress();

private:
	int   m_i;
	int   m_j;
	char  m_c;
	int   m_k;
	float m_f;
};

void ClassMemberTest();

#endif //!__CLASS_MEMBER_H__