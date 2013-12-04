// RegexpProxy.h: interface for the CRegexpProxy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGEXPPROXY_H__168C6EFF_1AFF_4A36_B1D7_2271E77AB0EC__INCLUDED_)
#define AFX_REGEXPPROXY_H__168C6EFF_1AFF_4A36_B1D7_2271E77AB0EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "regex.h"

class CRegexpProxy
{
public:
	int regcomp(regex_t *preg, const char *regex, int cflags);
	int regexec(const regex_t *preg, const char *string, size_t nmatch,regmatch_t pmatch[], int eflags);
	int match(char *string, char *regex);
	void regfree(regex_t *preg);
	int regnexec(const regex_t *preg, const char *string, size_t len, size_t nmatch, regmatch_t pmatch[], int eflags);
	int regncomp(regex_t *preg, const char *regex, size_t len, int cflags);
	CRegexpProxy();
	virtual ~CRegexpProxy();

private:
	HMODULE dllRegexp;
};

#endif // !defined(AFX_REGEXPPROXY_H__168C6EFF_1AFF_4A36_B1D7_2271E77AB0EC__INCLUDED_)
