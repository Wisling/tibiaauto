// AutoResponderParser.h: interface for the CAutoResponderParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTORESPONDERPARSER_H__65732CFB_96DC_4E25_A428_D55945E94058__INCLUDED_)
#define AFX_AUTORESPONDERPARSER_H__65732CFB_96DC_4E25_A428_D55945E94058__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "AutoResponderParserContext.h"
#include "ToolAutoResponderThreadConfig.h"
#include "RegexpProxy.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

class CAutoResponderParser
{
public:



	
	void parseThread(DOMNode *node,CAutoResponderParserContext *context);
	CAutoResponderParser(CToolAutoResponderThreadConfig *config);
	virtual ~CAutoResponderParser();
private:
	CRegexpProxy regexpProxy;
	int processNodeBoolNot(DOMNode *node, CAutoResponderParserContext *context);
	void breakProcessing(DOMNode *node);
	void processAction(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeDance(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeSetvariable(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeIgnoreplayer(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeRepeat(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeRandom(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeIf(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeSleep(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeSay(DOMNode *node,CAutoResponderParserContext *context);
	void processNodeSequence(DOMNode *node, CAutoResponderParserContext *context);
	int processNodeBoolTestvariable(DOMNode *node,CAutoResponderParserContext *context);
	int processNodeBoolPlayername(DOMNode *node,CAutoResponderParserContext *context);
	int processNodeBoolChannel(DOMNode *node,CAutoResponderParserContext *context);
	int processNodeBoolOr(DOMNode *node,CAutoResponderParserContext *context);
	int processNodeBoolAnd(DOMNode *node,CAutoResponderParserContext *context);
	int processNodeBoolCondition(DOMNode *node,CAutoResponderParserContext *context);
	int processNodeBoolMessage(DOMNode *node, CAutoResponderParserContext *context);
	void registerDebug(char *msg,CAutoResponderParserContext *context);
	void registerWarn(char *msg,CAutoResponderParserContext *context);
	void replaceSpecialStrings(char *msg,CAutoResponderParserContext *context);

	CToolAutoResponderThreadConfig *config;
};

#endif // !defined(AFX_AUTORESPONDERPARSER_H__65732CFB_96DC_4E25_A428_D55945E94058__INCLUDED_)
