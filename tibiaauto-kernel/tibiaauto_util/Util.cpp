// Util.cpp: implementation of the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Util.h"
#include "PQI.h"
#include <xercesc/util/XMLString.hpp>
#include "TibiaContainer.h"
#include "TibiaMap.h"
#include "PackSender.h"
#include "MemReader.h"
#include <string>

extern CTibiaMap tibiaMap;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtil::CUtil()
{

}

CUtil::~CUtil()
{

}



char *CUtil::wc2c(const XMLCh *src)
{
	char *dest=(char *)malloc(MAX_STRING_LEN);
	memset(dest,0,MAX_STRING_LEN);
	if (src==NULL)
		return dest;
	WideCharToMultiByte( CP_ACP, 0, src, -1,dest, MAX_STRING_LEN/2-10, NULL, NULL );
	return dest;
}

char * CUtil::getNodeAttribute(DOMNode *node, char *attrName)
{
	int attrNr;
	int attrCount=node->getAttributes()->getLength();
	

	for (attrNr=0;attrNr<attrCount;attrNr++)
	{
		DOMNode *attrNode = node->getAttributes()->item(attrNr);
		char *attrNameWide=CUtil::wc2c(attrNode->getNodeName());
		if (!strcmp(attrNameWide,attrName))
		{
			//free(attrNameWide);
			return CUtil::wc2c(attrNode->getNodeValue());
		}
		//free(attrNameWide);
	}
	char *emptyRet=(char *)malloc(MAX_STRING_LEN);
	memset(emptyRet,0,MAX_STRING_LEN);
	return emptyRet;
	
}

char *CUtil::getNodeAttribute(DOMNode *node, unsigned short *attrName)
{
	DOMNode *attrNode = node->getAttributes()->getNamedItem(attrName);
	if (attrNode)
	{
		return CUtil::wc2c(attrNode->getNodeValue());
	} else {
		char *emptyRet=(char *)malloc(1);
		memset(emptyRet,0,1);
		return emptyRet;
	}
}

int CUtil::getNodeIntAttribute(DOMNode *node, char *attrName)
{
	char *attrValue=getNodeAttribute(node,attrName);
	int ret=atoi(attrValue);
	//free(attrValue);
	return ret;
	
}

int CUtil::getNodeIntAttribute(DOMNode *node, unsigned short *attrName)
{
	DOMNode *attrNode = node->getAttributes()->getNamedItem((XMLCh*)attrName);
	if (attrNode)
	{
		char buf[128];
		buf[0]=0;
		wcstombs(buf,attrNode->getNodeValue(),60);
		return atoi(buf);
	} else {
		return 0;
	}
}

void CUtil::setNodeIntAttribute(DOMNode *node, char *attrName, int attr) {
	try {
		char buf[32];
		sprintf(buf, "%d", attr);
		XMLCh xmlStr[100];
		DOMNode *attrNode = node->getAttributes()->getNamedItem((XMLCh*)attrName);
		XMLString::transcode(buf, xmlStr, 99);
		attrNode->setNodeValue(xmlStr);
	}
	catch(...) {
		AfxMessageBox("Failed in setNodeIntAttribute");
	}
}

void CUtil::setNodeIntAttribute(DOMNode *node, unsigned short *attrName, int attr) {
	try {
		char buf[32];
		sprintf(buf, "%d", attr);
		XMLCh xmlStr[100];
		DOMNode *attrNode = node->getAttributes()->getNamedItem((XMLCh*)attrName);
		XMLString::transcode(buf, xmlStr, 99);
		attrNode->setNodeValue(xmlStr);
	}
	catch (...) {
		AfxMessageBox("Failed in setNodeIntAttribute");
	}
}