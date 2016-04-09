// ConfigCreatorUtilMultiParams.h: interface for the CConfigCreatorUtilMultiParams class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGCREATORUTILMULTIPARAMS_H__98C86192_8964_42FF_BE29_F20F26EF2B90__INCLUDED_)
#define AFX_CONFIGCREATORUTILMULTIPARAMS_H__98C86192_8964_42FF_BE29_F20F26EF2B90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_MULTIPARAMS 50

class CConfigCreatorUtilMultiParams
{
public:
	void registerMultiParam(const char *name);
	int isKnownMultiParam(const char *name);
	CConfigCreatorUtilMultiParams();
	virtual ~CConfigCreatorUtilMultiParams();

private:
	int paramsCount;
	char *paramsList[MAX_MULTIPARAMS];
};

#endif // !defined(AFX_CONFIGCREATORUTILMULTIPARAMS_H__98C86192_8964_42FF_BE29_F20F26EF2B90__INCLUDED_)
