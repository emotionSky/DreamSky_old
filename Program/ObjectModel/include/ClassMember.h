#ifndef __CLASS_MEMBER_H__
#define __CLASS_MEMBER_H__

/**
 * 查看类的对象数据布局：
 * 1）vs查看方式：
 *    打开vs的开发人员命令行工具，进入到cpp文件所在目录，然后执行命令  cl /dl reportSingleClassLayout{Name} {file}
 *    示例：  cl /dl reportSingleClassLayoutClassMember ClassMember.cpp
 * 2）gcc查看方式：
 *    g++ -fdump-class-hierarchy -fsyntax-only {file}
 * 
 * 查看obj文件的内容：
 * 1）vs查看方式：
 *    打开vs的开发人员命令行工具，进入obj文件所在目录，然后执行命令  dumpbin /all {obj_file} > {name}.txt
 *    示例：  dumpbin /all ClassMember.obj > ClassMember.txt
 * 2）gcc查看方式：
 *    g++ -E hello.cpp -o hello.i  //类似上面的txt
 *    g++ -S hello.i -o hello.s    //汇编
 * 
 * 查看可执行文件的内存布局：
 * 1）vs查看方式：
 * 2）gcc查看方式：
 *    nm {name}  示例： nm ObjectModel
 */

class ClassMember
{
public:
	ClassMember();
	~ClassMember();

	void PrintMemberDistance();
	void PrintMemberAddress();

	void PrintInfo();
	void PrintNum(int num);

	void PointerFuncString(const char* str);
	void PointerFuncNum(int num);
	static void PointerFuncStatic(float f);

private:
	int   m_i;
	int   m_j;
	char  m_c;
	int   m_k;
	float m_f;
};

void ClassMemberTest();
void ClassFunctionCast();
void ClassFunctionPointer();

#endif //!__CLASS_MEMBER_H__