// ConfigCreatorUtilMultiParams.cpp: implementation of the CConfigCreatorUtilMultiParams class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tibiaauto.h"
#include "ConfigCreatorUtilMultiParams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigCreatorUtilMultiParams::CConfigCreatorUtilMultiParams()
{
	int i;
	for (i=0;i<MAX_MULTIPARAMS;i++)
		paramsList[i]=(char *)malloc(MAX_STRING_LEN);
	paramsCount=0;
}

CConfigCreatorUtilMultiParams::~CConfigCreatorUtilMultiParams()
{
	int i;
	for (i=0;i<MAX_MULTIPARAMS;i++)
		free(paramsList[i]);
}

int CConfigCreatorUtilMultiParams::isKnownMultiparam(char *name)
{
	int i;
	for (i=0;i<paramsCount;i++)
	{
		if (!strcmp(paramsList[i],name))
			return 1;
	}
	return 0;
}

void CConfigCreatorUtilMultiParams::registerMultiparam(char *name)
{
	if (paramsCount>=MAX_MULTIPARAMS)
		return;
	strcpy(paramsList[paramsCount++],name);
}
