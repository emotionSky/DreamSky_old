#ifndef __STRING_OPERATION_H__
#define __STRING_OPERATION_H__

#include "common_type.h"
#include <string>

/**
 * @brief ɾ���ַ�����ߵ��ض��ַ�
 * @param[in] str ��Ҫ�������ַ���
 * @param[in] ch  ��Ҫɾ�����ض��ַ�
 * @return void
 */
DREAMSKY_API void left_delete(char* str, char ch);

/**
 * @brief ɾ���ַ����ұߵ��ض��ַ�
 * @param[in] str ��Ҫ�������ַ���
 * @param[in] ch  ��Ҫɾ�����ض��ַ�
 * @return void
 */
DREAMSKY_API void right_delete(char* str, char ch);

#define left_trim(str) left_delete(str, ' ')
#define right_trim(str) right_delete(str, ' ')
#define all_trim(str) \
{ \
	left_delete(str, ' '); \
	right_delete(str, ' '); \
}

DREAMSKY_API void left_delete_string(std::string& str, char ch);

DREAMSKY_API void right_delete_string(std::string& str, char ch);

#define left_trim_string(str) left_delete_string(str, ' ')
#define right_trim_string(str) right_delete_string(str, ' ')
#define all_trim_string(str) \
{ \
	left_delete_string(str, ' '); \
	right_delete_string(str, ' '); \
}

#endif //!__STRING_OPERATION_H__