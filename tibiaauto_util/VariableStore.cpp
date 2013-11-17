// VariableStore.cpp: implementation of the CVariableStore class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto_util.h"
#include "VariableStore.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////

char CVariableStore::variableStore[MAX_VARIABLES][210];
char CVariableStore::variableStoreValues[MAX_VARIABLES][210];
int CVariableStore::variableCount=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVariableStore::CVariableStore()
{

}

CVariableStore::~CVariableStore()
{

}

void CVariableStore::setVariable(char *name, char *value)
{
	if (strlen(name)>200||strlen(value)>200) return;
	int idx=findVariableIndex(name);
	if (idx==-1)
	{
		if (variableCount<MAX_VARIABLES)
		{
			idx=variableCount++;
			sprintf(variableStore[idx],"%s",name);
			sprintf(variableStoreValues[idx],"%s",value);
		} else {
			// too many variables - do nothing
		} 
	} else {
		// update value of variable
		sprintf(variableStoreValues[idx],"%s",value);
	}
}

//never returns null string(anymore)
const char * CVariableStore::getVariable(char *name)
{
	
	int idx=findVariableIndex(name);
	if (idx==-1){
		return "";
	}
	return variableStoreValues[idx];
}

int CVariableStore::findVariableIndex(char *name)
{
	int i;
	for (i=0;i<MAX_VARIABLES;i++)
	{
		if (!strcmpi(name,variableStore[i]))
			return i;
	}
	return -1;
}
