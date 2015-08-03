#pragma once
#include "tibiaauto_util.h"

#include "TibiaItem.h"
#include "PQI.h"


#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TIBIAAUTOUTIL_API CUtil
{
public:
	static char * getNodeAttribute(DOMNode *node, char *attrName);
	static int getNodeIntAttribute(DOMNode *node, char *attrName);
	static char * getNodeAttribute(DOMNode *node, unsigned short *attrName);
	static int getNodeIntAttribute(DOMNode *node, unsigned short *attrName);
	static void setNodeIntAttribute(DOMNode *node, char *attrName, int attr);
	static void setNodeIntAttribute(DOMNode *node, unsigned short *attrName, int attr);
	static char *CUtil::wc2c(const XMLCh *src);

	CUtil();
	virtual ~CUtil();
};
