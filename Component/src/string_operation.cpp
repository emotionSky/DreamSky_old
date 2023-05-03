#include <Component/general/string_operation.h>
#include <cstring>

void left_delete(char* str, char ch)
{
	size_t len = 0;
	if (str == nullptr)
		return;

	len = strlen(str);
	while (len > 0 && str[len - 1] == ch)   //λ�û�һ��   
		str[--len] = 0;
}

void right_delete(char* str, char ch)
{
	size_t len = 0;
	len = strlen(str);
	char* p_tmp = str;

	/* �������ֱ����ch��ͷ�ģ����ؼ��� */
	if ((*p_tmp) != ch)
		return;

	/* �ҵ���һ������ch��λ�� */
	while ((*p_tmp) != '\0')
	{
		if ((*p_tmp) == ch)
			p_tmp++;
		else
			break;
	}

	/* ����ַ���ȫ������ch�ַ����ַ�����շ��� */
	if ((*p_tmp) == '\0')
	{
		*str = '\0';
		return;
	}

	char* p_tmp2 = str;
	while ((*p_tmp) != '\0')
	{
		(*p_tmp2) = (*p_tmp);
		p_tmp++;
		p_tmp2++;
	}
	(*p_tmp2) = '\0';
}

void left_delete_string(std::string& str, char ch)
{
	if (str.empty())
		return;
	
	/* �ж��Ƿ�Ϊƥ����ַ� */
	auto IsMatched = [ch](char ucs4) ->bool {
		/**
		 * 0x20	space �ո�
		 * 0x09 HT - horizontal tab ˮƽ�Ʊ��
		 * 0x0A LF - line feed ���м�
		 * 0x0B VT - vertical tab ��ֱ�Ʊ��
		 * 0x0C FF - form feed ��ҳ��
		 * 0x0D CR - carriage return �س���
		 */
		//return ucs4 == 0x20 || (ucs4 <= 0x0d && ucs4 >= 0x09);
		return ucs4 == ch;
	};

	auto begin = str.begin();
	auto end = str.end();

	while ((begin < end) && IsMatched(*begin))
		begin++;
	str = std::string(begin, end);
}

void right_delete_string(std::string& str, char ch)
{
	if (str.empty())
		return;

	/* �ж��Ƿ�Ϊƥ����ַ� */
	auto IsMatched = [ch](char ucs4) ->bool {
		/**
		 * 0x20	space �ո�
		 * 0x09 HT - horizontal tab ˮƽ�Ʊ��
		 * 0x0A LF - line feed ���м�
		 * 0x0B VT - vertical tab ��ֱ�Ʊ��
		 * 0x0C FF - form feed ��ҳ��
		 * 0x0D CR - carriage return �س���
		 */
		 //return ucs4 == 0x20 || (ucs4 <= 0x0d && ucs4 >= 0x09);
		return ucs4 == ch;
	};

	auto begin = str.begin();
	auto end = str.end();
	while ((begin < end) && IsMatched(end[-1]))
		--end;

	str = std::string(begin, end);
}