// Util.h: interface for the CUtil class.
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
#if XERCES_VERSION_MAJOR < 3
#include <xercesc/dom/DOMWriter.hpp>
#endif

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
	static int getNodeIntAttribute(DOMNode *node,char *attrName);
	static char * getNodeAttribute(DOMNode *node,unsigned short *attrName);
	static int getNodeIntAttribute(DOMNode *node, unsigned short *attrName);
	static void setNodeIntAttribute(DOMNode *node,char *attrName, int attr);
	static void setNodeIntAttribute(DOMNode *node, unsigned short *attrName, int attr);
	static char *CUtil::wc2c(const XMLCh *src);

	CUtil();
	virtual ~CUtil();

};

#endif // !defined(AFX_UTIL_H__79D8A6EB_5316_41E0_BA36_717AF72A873E__INCLUDED_)
