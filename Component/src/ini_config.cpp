#include <Component/config/ini_config.h>
#include <Component/general/string_operation.h>
//#include <Log/log_core.h>
#include <cstdio>
#include <map>
#include <string>
#include <cstring>

class ConfigData
{
public:
	static std::string    m_path;
	std::map<std::string, std::string> m_str;
};

std::string ConfigData::m_path = "./config/config.ini";

IniConfig::IniConfig() :
	m_pData(new ConfigData)
{
	m_bInit = false;
}

IniConfig* IniConfig::Instance()
{
	static IniConfig ini;
	return &ini;
}

IniConfig::~IniConfig()
{
	if (m_pData)
	{
		delete m_pData;
		m_pData = nullptr;
	}
}

bool IniConfig::LoadConfig()
{
	if (m_bInit)
		return true;

	FILE* fp = fopen(m_pData->m_path.c_str(), "r");
	if (!fp)
	{
		//log_printf(LOG_ERROR, "failed to open file:%s.", m_pData->m_path.c_str());
		return false;
	}

	/* 将读取的每一行内容存放在buff中 */
	char linebuf[501]; //每行配置都不要太长，保持<500字符内，防止出现问题
	char key[50];      //存放每一个读取的键
	char value[500];   //存放每一个读取的值

	/* 读取文件，知道文件结束 */
	while (!feof(fp))
	{
		/* 这里就保证了每一次的读取不超过500个字符 */
		if (fgets(linebuf, 500, fp) == NULL) //从文件中读数据，每次读一行，一行最多不要超过500个字符 
			continue;

		if (linebuf[0] == 0)
			continue;

		/* 处理注释行，这里就是需要约定一些注释的符号用什么 */
		if (*linebuf == ';' || *linebuf == ' ' || *linebuf == '#' || *linebuf == '\t' || *linebuf == '\n')
			continue;

	lblprocstring:
		/* 处理最后的字符，如果是换行、回车、空格都需要干掉 */
		if (strlen(linebuf) > 0)
		{
			if (linebuf[strlen(linebuf) - 1] == 10 /* '\n' */ || linebuf[strlen(linebuf) - 1] == 13 /* '\r */ || linebuf[strlen(linebuf) - 1] == 32 /* ' ' */)
			{
				linebuf[strlen(linebuf) - 1] = 0;
				goto lblprocstring;
			}
		}

		/* 说明处理完了之后，这个字符串是一个空字符串 */
		if (linebuf[0] == 0)
			continue;

		/* 特殊字符串的处理 */
		if (*linebuf == '[')
			continue;

		/* 符合 xx=xx 格式的才能继续处理 */
		char* ptmp = strchr(linebuf, '=');
		if (ptmp != nullptr)
		{
			memset(key, 0, sizeof(key));
			memset(value, 0, sizeof(value));
			strncpy(key, linebuf, ptmp - linebuf);
			strcpy(value, ptmp + 1);

			all_trim(key);
			all_trim(value);

			m_pData->m_str.insert(std::make_pair<>(std::string(key), std::string(value)));
		}
	}

	fclose(fp);
	m_bInit = true;
	
	return true;
}

const char* IniConfig::GetString(const char* key)
{
	if (key)
	{
		const auto& data = m_pData->m_str;
		auto iter = data.find(std::string(key));
		if (iter != data.end())
			return iter->second.c_str();
	}
	return nullptr;
}

int IniConfig::GetInt(const char* key, int def)
{
	if (key)
	{
		const auto& data = m_pData->m_str;
		auto iter = data.find(std::string(key));
		if (iter != data.end())
			return std::stoi(iter->second);
	}
	return def;
}

#ifdef ALLOW_CONFIG_CHANGE
bool IniConfig::SaveConfig()
{
	FILE* fp = fopen(m_pData->m_path.c_str(), "w");
	if (!fp)
	{
		log_printf(LOG_ERROR, "failed to open file:%s.", m_pData->m_path.c_str());
		return false;
	}

	const auto& data = m_pData->m_str;
	for (auto iter = data.cbegin(); iter != data.cend(); ++iter)
		fprintf(fp, "%s=%s\n", iter->first.c_str(), iter->second.c_str());

	fclose(fp);

	return true;
}

void IniConfig::SetString(const char* key, const char* value)
{
	auto& data = m_pData->m_str;
	data[std::string(key)] = std::string(value);
}

void IniConfig::SetInt(const char* key, int value)
{
	auto& data = m_pData->m_str;
	data[std::string(key)] = std::to_string(value);
}

#endif