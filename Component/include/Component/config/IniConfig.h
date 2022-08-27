#ifndef __INI_CONFIG_H__
#define __INI_CONFIG_H__

#include "common_type.h"

class DREAMSKY_API IniConfig
{
private:
	IniConfig();

public:
	static IniConfig* Instance();
	~IniConfig();
};

#endif //!__INI_CONFIG_H__