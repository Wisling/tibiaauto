#pragma once
#include "tibiaauto_util.h"

#define MAX_VARIABLES 100

class TIBIAAUTOUTIL_API CVariableStore
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
