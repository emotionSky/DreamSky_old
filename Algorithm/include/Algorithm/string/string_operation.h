#ifndef __STRING_OPERATION_H__
#define __STRING_OPERATION_H__

#include "common_type.h"
#include <string>

/**
 * @brief 删除字符串左边的特定字符
 * @param[in] str 需要操作的字符串
 * @param[in] ch  需要删除的特定字符
 * @return void
 */
DREAMSKY_API void left_delete(char* str, char ch);

/**
 * @brief 删除字符串右边的特定字符
 * @param[in] str 需要操作的字符串
 * @param[in] ch  需要删除的特定字符
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