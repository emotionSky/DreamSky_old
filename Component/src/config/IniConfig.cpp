#include "Component/config/IniConfig.h"

IniConfig::IniConfig()
{

}

IniConfig* IniConfig::Instance()
{
	static IniConfig ini;
	return &ini;
}

IniConfig::~IniConfig()
{

}