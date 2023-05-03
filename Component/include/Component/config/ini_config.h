#ifndef __INI_CONFIG_H__
#define __INI_CONFIG_H__

#include <Component/dream_component.h>

/**
 * @class ConfigData
 * @brief 防止配置文件数据的类，兼容stl的导出
 */
class ConfigData;

/**
 * @class IniConfig
 * @brief 配置文件单例类
 */
class DREAMSKY_API IniConfig
{
private:
	IniConfig();

public:
	static IniConfig* Instance();
	~IniConfig();

	bool LoadConfig();
	const char* GetString(const char* key);
	int GetInt(const char* key, int def);

#ifdef ALLOW_CONFIG_CHANGE
	bool SaveConfig();
	void SetString(const char* key, const char* value);
	void SetInt(const char* key, int value);
#endif

private:
	ConfigData* m_pData;
	bool        m_bInit;
};

#endif //!__INI_CONFIG_H__