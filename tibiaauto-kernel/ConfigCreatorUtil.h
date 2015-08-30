// ConfigCreatorUtil.h: interface for the CConfigCreatorUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGCREATORUTIL_H__9BC2F130_36AF_43E2_B3CB_C27073E3FCED__INCLUDED_)
#define AFX_CONFIGCREATORUTIL_H__9BC2F130_36AF_43E2_B3CB_C27073E3FCED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ModuleLoader.h"
#include "ConfigCreatorUtilMultiParams.h"
#include "PythonScript.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>


XERCES_CPP_NAMESPACE_USE

class CConfigCreatorUtil
{
public:
	void parseConfigFromNode(CPythonScript *pythonScript, DOMNode *node, char *paramName);
	void parseConfigFromNode(IModuleInterface *module, DOMNode *node, char *paramName);
	DOMNode * getConfigForModule(DOMElement *root, const char *moduleName);
	void releaseConfig(DOMNode *modConfig);
	void addParamToConfig(DOMNode *node, char *paramName, char *paramValue);
	void serializeConfigForModule(DOMNode *modNode, FILE *f);
	DOMNode * getEmptyConfigForModule(const char *moduleName);
	CConfigCreatorUtil();
	virtual ~CConfigCreatorUtil();

	CConfigCreatorUtilMultiParams *multiParams;
private:
	static void addParamFromNode(xercesc::DOMDocument *doc, DOMNode *modConfig, char *paramName, char *paramValue);
};

#endif // !defined(AFX_CONFIGCREATORUTIL_H__9BC2F130_36AF_43E2_B3CB_C27073E3FCED__INCLUDED_)
