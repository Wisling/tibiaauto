//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Util.h"
#include "PQI.h"

#include "TibiaContainer.h"
#include "TibiaMapProxy.h"
#include "PackSenderProxy.h"
#include "MemReaderProxy.h"
#include "TibiaMapPoint.h"


#define MAX_STRING_LEN 65536

extern CTibiaMapProxy tibiaMap;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtil::CUtil()
{
}

CUtil::~CUtil()
{
}

char *CUtil::wc2c(const unsigned short *src)
{
	char *dest = (char *)malloc(MAX_STRING_LEN);
	memset(dest, 0, MAX_STRING_LEN);
	if (src == NULL)
		return dest;
	WideCharToMultiByte(CP_ACP, 0, src, -1, dest, MAX_STRING_LEN / 2 - 10, NULL, NULL);
	return dest;
}

char * CUtil::getNodeAttribute(DOMNode *node, char *attrName)
{
	size_t attrNr;
	for (attrNr = 0; attrNr < node->getAttributes()->getLength(); attrNr++)
	{
		DOMNode *attrNode  = node->getAttributes()->item(attrNr);
		char *attrNameWide = CUtil::wc2c(attrNode->getNodeName());
		if (!strcmp(attrNameWide, attrName))
		{
			free(attrNameWide);
			return CUtil::wc2c(attrNode->getNodeValue());
		}
		free(attrNameWide);
	}
	char *emptyRet = (char *)malloc(MAX_STRING_LEN);
	memset(emptyRet, 0, MAX_STRING_LEN);
	return emptyRet;
}
