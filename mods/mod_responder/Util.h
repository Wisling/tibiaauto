//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTIL_H__79D8A6EB_5316_41E0_BA36_717AF72A873E__INCLUDED_)
#define AFX_UTIL_H__79D8A6EB_5316_41E0_BA36_717AF72A873E__INCLUDED_

#include "TibiaItem.h"
#include "PQI.h"


#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUtil
{
public:
	static char * getNodeAttribute(DOMNode *node,char *attrName);
	static char *wc2c(const unsigned short *src);
	CUtil();
	virtual ~CUtil();


};

#endif // !defined(AFX_UTIL_H__79D8A6EB_5316_41E0_BA36_717AF72A873E__INCLUDED_)
