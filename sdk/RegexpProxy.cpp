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
	dllRegexp=LoadLibrary("mods\\tre.dll");
}

CRegexpProxy::~CRegexpProxy()
{

}

int CRegexpProxy::match(char *string, char *regex)
{
	regex_t preg;
	if (!regcomp(&preg,regex,REG_EXTENDED|REG_ICASE|REG_NOSUB))
	{
		if (!regexec(&preg,string,0,NULL,NULL))
		{
			// match found
			regfree(&preg);
			return 1;
		} else {
			// no match found
			regfree(&preg);
			return 0;
		}
	} else {
		// error during compilation of regex
		return 0;
	}
}

int CRegexpProxy::regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags)
{	
	if (dllRegexp)
	{
		typedef int(*Proto_fun)(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags);	
		Proto_fun fun=(Proto_fun)GetProcAddress(dllRegexp,"regexec");
		if (fun)
		{
			return fun(preg,string,nmatch,pmatch,eflags);
		} else {
			return 1;
		}	
	} else {
		return 1;
	}
}

int CRegexpProxy::regcomp(regex_t *preg, const char *regex, int cflags)
{
	if (dllRegexp)
	{
		typedef int(*Proto_fun)(regex_t *preg, const char *regex, int cflags);	
		Proto_fun fun=(Proto_fun)GetProcAddress(dllRegexp,"regcomp");
		if (fun)
		{
			return fun(preg,regex,cflags);
		} else {
			return REG_NOMATCH;
		}	
	} else {
		return REG_NOMATCH;
	}
}

void CRegexpProxy::regfree(regex_t *preg)
{
	if (dllRegexp)
	{
		typedef int(*Proto_fun)(regex_t *preg);	
		Proto_fun fun=(Proto_fun)GetProcAddress(dllRegexp,"regfree");
		if (fun)
		{
			fun(preg);
		}	
	}
}


