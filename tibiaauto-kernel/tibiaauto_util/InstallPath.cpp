#include "stdafx.h"
#include "InstallPath.h"

static bool hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

CInstallPath::CInstallPath()
{
	valid = false;
	TCHAR modulePath[512];
	if (!GetModuleFileName(NULL, modulePath, sizeof(modulePath) / sizeof(TCHAR)))
		return;
	std::string sModulePath = modulePath;
	if (hasEnding(sModulePath, "Tibia.exe"))
	{
		HMODULE hInjectDll = LoadLibrary((LPCTSTR) "tibiaautoinject2.dll");
		if (!hInjectDll)
			return;
		if (!GetModuleFileName(hInjectDll, modulePath, sizeof(modulePath) / sizeof(TCHAR)))
			return;
		sModulePath = modulePath;
	}
	installPath.assign(sModulePath.substr(0, sModulePath.find_last_of("\\/")));
	valid = true;
}


CInstallPath::~CInstallPath()
{
}
