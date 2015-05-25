// VariableStore.h: interface for the CVariableStore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARIABLESTORE_H__A09A3B1B_E3C8_4927_AA93_5EBEB2F8E088__INCLUDED_)
#define AFX_VARIABLESTORE_H__A09A3B1B_E3C8_4927_AA93_5EBEB2F8E088__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_VARIABLES 100

class CVariableStore
{
public:
	static const char * getVariable(char *name);
	static void setVariable(char *name, char *value);
	CVariableStore();
	virtual ~CVariableStore();

private:
	static int findVariableIndex(char *name);
	static char variableStore[MAX_VARIABLES][210];
	static char variableStoreValues[MAX_VARIABLES][210];
	static int variableCount;
};

#endif // !defined(AFX_VARIABLESTORE_H__A09A3B1B_E3C8_4927_AA93_5EBEB2F8E088__INCLUDED_)
