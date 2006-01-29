// Util.h: interface for the CUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTIL_H__79D8A6EB_5316_41E0_BA36_717AF72A873E__INCLUDED_)
#define AFX_UTIL_H__79D8A6EB_5316_41E0_BA36_717AF72A873E__INCLUDED_

#include "TibiaItem.h"
#include "Queue.h"


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
	static int getNodeIntAttribute(DOMNode *node,char *attrName);
	static char * getNodeAttribute(DOMNode *node,unsigned short *attrName);
	static int getNodeIntAttribute(DOMNode *node,unsigned short *attrName);
	static void lootItemFromContainer(int conrNr, CUIntArray *acceptedItems);
	static int waitForOpenContainer(int contNr,int open);
	static void findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[5000]);
	static void findPathOnMap(int startX, int startY, int startZ, int endX, int endY, int endZ, int path[5000],int allowStepOnGoUp);
	static CTibiaItem * lookupItem(int containerNr, CUIntArray *itemsAccepted);
	static CTibiaItem * CUtil::lookupItem(int containerNr, CUIntArray *itemsAccepted,int qty);
	static char *CUtil::wc2c(const unsigned short *src);
	CUtil();
	virtual ~CUtil();


};

#endif // !defined(AFX_UTIL_H__79D8A6EB_5316_41E0_BA36_717AF72A873E__INCLUDED_)
