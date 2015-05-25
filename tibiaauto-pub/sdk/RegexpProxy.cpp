// RegexpProxy.cpp: implementation of the CRegexpProxy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegexpProxy.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegexpProxy::CRegexpProxy()
{
	char installPath[1024];
	unsigned long installPathLen = 1023;
	installPath[0] = '\0';
	HKEY hkey = NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_READ, &hkey))
	{
		RegQueryValueEx(hkey, TEXT("Install_Dir"), NULL, NULL, (unsigned char *)installPath, &installPathLen);
		RegCloseKey(hkey);
	}
	if (!strlen(installPath))
	{
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		PostQuitMessage(-1);
		return;
	}

	char pathBuf[2048];
	sprintf(pathBuf, "%s\\mods\\tre.dll", installPath);

	dllRegexp = LoadLibrary(pathBuf);
}

CRegexpProxy::~CRegexpProxy()
{
}

int CRegexpProxy::match(char *string, char *regex)
{
	regex_t preg;
	if (!regcomp(&preg, regex, REG_EXTENDED | REG_ICASE | REG_NOSUB))
	{
		if (!regexec(&preg, string, 0, NULL, NULL))
		{
			// match found
			regfree(&preg);
			return 1;
		}
		else
		{
			// no match found
			regfree(&preg);
			return 0;
		}
	}
	else
	{
		// error during compilation of regex
		return 0;
	}
}

int CRegexpProxy::regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags)
{
	if (dllRegexp)
	{
		typedef int (*Proto_fun)(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllRegexp, "regexec");
		if (fun)
			return fun(preg, string, nmatch, pmatch, eflags);
		else
			return 1;
	}
	else
	{
		return 1;
	}
}

int CRegexpProxy::regcomp(regex_t *preg, const char *regex, int cflags)
{
	if (dllRegexp)
	{
		typedef int (*Proto_fun)(regex_t *preg, const char *regex, int cflags);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllRegexp, "regcomp");
		if (fun)
			return fun(preg, regex, cflags);
		else
			return REG_NOMATCH;
	}
	else
	{
		return REG_NOMATCH;
	}
}

void CRegexpProxy::regfree(regex_t *preg)
{
	if (dllRegexp)
	{
		typedef int (*Proto_fun)(regex_t *preg);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllRegexp, "regfree");
		if (fun)
			fun(preg);
	}
}

int CRegexpProxy::regnexec(const regex_t *preg, const char *string, size_t len, size_t nmatch, regmatch_t pmatch[], int eflags)
{
	if (dllRegexp)
	{
		typedef int (*Proto_fun)(const regex_t *preg, const char *string, size_t len, size_t nmatch, regmatch_t pmatch[], int eflags);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllRegexp, "regnexec");
		if (fun)
			return fun(preg, string, len, nmatch, pmatch, eflags);
		else
			return 1;
	}
	else
	{
		return 1;
	}
}

int CRegexpProxy::regncomp(regex_t *preg, const char *regex, size_t len, int cflags)
{
	if (dllRegexp)
	{
		typedef int (*Proto_fun)(regex_t *preg, const char *regex, size_t len, int cflags);
		static Proto_fun fun = (Proto_fun)GetProcAddress(dllRegexp, "regncomp");
		if (fun)
			return fun(preg, regex, len, cflags);
		else
			return REG_NOMATCH;
	}
	else
	{
		return REG_NOMATCH;
	}
}
