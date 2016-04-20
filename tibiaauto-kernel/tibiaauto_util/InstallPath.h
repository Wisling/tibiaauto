#pragma once
#include "tibiaauto_util.h"
#include <string>

class TIBIAAUTOUTIL_API CInstallPath
{
private:
	CInstallPath();
	~CInstallPath();
	CInstallPath(CInstallPath const&) { };
	bool valid;
	std::string installPath;
public:
	static CInstallPath& getInstallPath()
	{
		static CInstallPath singleton;
		return singleton;
	}
	const char* c_str() { return installPath.c_str(); }
	bool isValid() { return valid; }
};

