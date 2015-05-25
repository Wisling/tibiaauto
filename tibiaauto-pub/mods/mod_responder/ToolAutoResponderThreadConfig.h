// ToolAutoResponderThreadConfig.h: interface for the CToolAutoResponderThreadConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLAUTORESPONDERTHREADCONFIG_H__80B5E409_9D57_48A8_B8AB_D417A100FF8A__INCLUDED_)
#define AFX_TOOLAUTORESPONDERTHREADCONFIG_H__80B5E409_9D57_48A8_B8AB_D417A100FF8A__INCLUDED_

#include "AutoResponderParserContext.h" // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolAutoResponderMessage.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <queue>

XERCES_CPP_NAMESPACE_USE


enum CToolAutoResponderThreadStatus
{
	CToolAutoResponderThreadStatus_notRunning,
	CToolAutoResponderThreadStatus_waitingForMatch,
	CToolAutoResponderThreadStatus_generalParse,
	CToolAutoResponderThreadStatus_sleep
};

class CToolAutoResponderThreadConfig
{
public:
	CToolAutoResponderThreadConfig();
	virtual ~CToolAutoResponderThreadConfig();

	CAutoResponderParserContext *context;
	DOMNode *threadNode;
	std::queue<CToolAutoResponderMessage *> queue;
	CToolAutoResponderThreadStatus status;
};

#endif // !defined(AFX_TOOLAUTORESPONDERTHREADCONFIG_H__80B5E409_9D57_48A8_B8AB_D417A100FF8A__INCLUDED_)
