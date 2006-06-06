// AutoResponderParser.cpp: implementation of the CAutoResponderParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_responder.h"
#include "AutoResponderParser.h"
#include "ModuleUtil.h"
#include "Util.h"
#include "PackSenderProxy.h"
#include "MemReaderProxy.h"
#include "TibiaCharacter.h"
#include "AliceProxy.h"


XERCES_CPP_NAMESPACE_USE


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 

int autoResponderStop=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoResponderParser::CAutoResponderParser(CToolAutoResponderThreadConfig *config)
{
	this->config=config;
}

CAutoResponderParser::~CAutoResponderParser()
{

}


void CAutoResponderParser::parseThread(DOMNode *node,CAutoResponderParserContext *context)
{					
	char *selfNodeNameTmp=CUtil::wc2c(node->getNodeName());	
	char *selfNodeName=(char *)malloc(MAX_STRING_LEN);
	int i,len;
	memcpy(selfNodeName,selfNodeNameTmp,strlen(selfNodeNameTmp)+1);
	free(selfNodeNameTmp);
	for (i=0,len=strlen(selfNodeName);i<len;i++)
		selfNodeName[i]=tolower(selfNodeName[i]);		
	
	
	if (strcmp(selfNodeName,"thread"))
	{		
		free(selfNodeName);
		breakProcessing(node);	
		return;
	}
	free(selfNodeName);

	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);								

		processAction(childNode,context);				
	};				
}

void CAutoResponderParser::processNodeSay(DOMNode *node, CAutoResponderParserContext *context)
{
	registerDebug("DEBUG: action: say",context);
	CPackSenderProxy sender;
	int useAlice=0;	
	char *sayText=CUtil::getNodeAttribute(node,"text");		
	char *channelText=CUtil::getNodeAttribute(node,"channel");		
	char *aliceText=CUtil::getNodeAttribute(node,"alice");		
	replaceSpecialStrings(sayText,context);

	if (!strcmp(aliceText,"true")||!strcmp(aliceText,"yes"))
	{
		useAlice=1;
	}
	free(aliceText);

	if (useAlice)
	{
		CAliceProxy alice;
		registerDebug("DEBUG: action: calling Aliece",context);

		char *resp=alice.respond(context->message,context->playerName);
		sprintf(sayText,resp);	
		delete resp;
	}
	
	int chanHit=0;

	if (strlen(channelText)==0||!strcmp(channelText,"say"))
	{
		chanHit=1;
		sender.say(sayText);	
		if (context->localEcho)
		{
			char *senderBuf=(char *)malloc(MAX_STRING_LEN);
			sprintf(senderBuf,"[responder] {say} %s",sayText);
			sender.sendTAMessage(senderBuf);
			free(senderBuf);
		}
	}
	if (!strcmp(channelText,"private")||!strcmp(channelText,"priv"))
	{
		chanHit=1;
		char *playerText=CUtil::getNodeAttribute(node,"player");		
		replaceSpecialStrings(playerText,context);
		sender.tell(sayText,playerText);
		if (context->localEcho)
		{
			char *senderBuf=(char *)malloc(MAX_STRING_LEN);
			sprintf(senderBuf,"[responder] {%s} %s",playerText,sayText);
			sender.sendTAMessage(senderBuf);
			free(senderBuf);
		}
		if (!strlen(playerText))
		{
			registerWarn("WARN: player name is 0 length",context);
		}
		free(playerText);
	}
	
	if (!chanHit)
	{
		registerWarn("DEBUG: say channel unknown",context);
	}
	if (!strlen(sayText))
	{
		registerWarn("DEBUG: say text is 0 length", context);
	}

	free(sayText);
	free(channelText);
}

void CAutoResponderParser::processNodeSleep(DOMNode *node, CAutoResponderParserContext *context)
{		
	char *timeText=CUtil::getNodeAttribute(node,"time");			
	int sleepTimeLeft=atoi(timeText);

	char *buf=(char *)malloc(MAX_STRING_LEN);
	sprintf(buf,"DEBUG: action: sleep: time=%d",sleepTimeLeft);
	registerDebug(buf,context);

	CToolAutoResponderThreadStatus prevStatus=config->status;
	config->status=CToolAutoResponderThreadStatus_sleep;
	while (sleepTimeLeft>0&&!autoResponderStop)
	{
		if (sleepTimeLeft>100)
		{
			Sleep(100);
			sleepTimeLeft-=100;
		} else {
			Sleep(sleepTimeLeft);
			sleepTimeLeft=0;
		}		
	}
	config->status=prevStatus;
	free(timeText);
	free(buf);
}

void CAutoResponderParser::processNodeIf(DOMNode *node, CAutoResponderParserContext *context)
{
	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);

		if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;
		
		char *nodeNameTmp=CUtil::wc2c(childNode->getNodeName());	
		char *nodeName=(char *)malloc(MAX_STRING_LEN);
		int i,len;
		memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
		free(nodeNameTmp);
		for (i=0,len=strlen(nodeName);i<len;i++)
			nodeName[i]=tolower(nodeName[i]);
		
		if (!strcmp(nodeName,"condition"))
		{
			if (!processNodeBoolCondition(childNode,context)) 
			{
				char *buf=(char *)malloc(MAX_STRING_LEN);
				sprintf(buf,"DEBUG: 'if' condition failed");
				registerDebug(buf,context);
				free(buf);
				free(nodeName);
				return;
			} 
		}
		free(nodeName);
	}
	registerDebug("DEBUG: 'if' all conditions passed",context);

	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);

		if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;
		
		char *nodeNameTmp=CUtil::wc2c(childNode->getNodeName());	
		char *nodeName=(char *)malloc(MAX_STRING_LEN);
		int i,len;
		memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
		free(nodeNameTmp);
		for (i=0,len=strlen(nodeName);i<len;i++)
			nodeName[i]=tolower(nodeName[i]);
		
		if (strcmp(nodeName,"condition"))
		{
			processAction(childNode,context);
		}
		free(nodeName);
	}
}

void CAutoResponderParser::processNodeRandom(DOMNode *node, CAutoResponderParserContext *context)
{
	registerDebug("DEBUG: action: random",context);
	int childNodesCount=0;
	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);

		if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;
		
		childNodesCount++;
	}
	if (childNodesCount)
	{
		int selectedNodeNr=rand()%childNodesCount;
		
		for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
		{
			DOMNode *childNode = node->getChildNodes()->item(nodeNr);

			if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;

			if (!selectedNodeNr)
			{
				// this is the selected node 
				processAction(childNode,context);
				return;
			}
			selectedNodeNr--;
		}
		
	}
}

void CAutoResponderParser::processNodeRepeat(DOMNode *node, CAutoResponderParserContext *context)
{	
	registerDebug("DEBUG: action: repeat",context);
	char *countText=CUtil::getNodeAttribute(node,"count");
	int countLeft=atoi(countText);
	int countTotal=atoi(countText);

	char *buf=(char *)malloc(MAX_STRING_LEN);
	sprintf(buf,"DEBUG: repeat %d times to process",countTotal);
	registerDebug(buf,context);	

	while (countLeft>0)
	{
		sprintf(buf,"DEBUG: repeating %d/%d",countTotal-countLeft+1,countTotal);
		registerDebug(buf,context);
		int nodeNr;
		for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
		{
			DOMNode *childNode = node->getChildNodes()->item(nodeNr);			
			
			processAction(childNode,context);		
		}
		countLeft--;
	}
	free(buf);
	free(countText);
}

void CAutoResponderParser::processNodeIgnoreplayer(DOMNode *node, CAutoResponderParserContext *context)
{
	registerDebug("DEBUG: action: ignoreplayer",context);

	char *nameText=CUtil::getNodeAttribute(node,"name");	
	replaceSpecialStrings(nameText,context);
	char *timeText=CUtil::getNodeAttribute(node,"time");	
	context->setIgnoredPlayer(nameText,time(NULL)+atoi(timeText));
	free(nameText);
	free(timeText);
}

void CAutoResponderParser::processNodeSetvariable(DOMNode *node, CAutoResponderParserContext *context)
{
	registerDebug("DEBUG: action: setvariable",context);
	char *nameText=CUtil::getNodeAttribute(node,"name");	
	replaceSpecialStrings(nameText,context);
	char *valueText=CUtil::getNodeAttribute(node,"value");	
	replaceSpecialStrings(valueText,context);
	context->setVariable(nameText,valueText);	
	free(nameText);
	free(valueText);
}

void CAutoResponderParser::processNodeDance(DOMNode *node, CAutoResponderParserContext *context)
{
	registerDebug("DEBUG: action: dance",context);
	CPackSenderProxy sender;
	char *countText=CUtil::getNodeAttribute(node,"count");	
	int count=atoi(countText);
	free(countText);
	int i;
	for (i=0;i<count;i++)
	{
		sender.turnLeft();
		Sleep(250);
		sender.turnRight();
		Sleep(250);
	}
}


int CAutoResponderParser::processNodeBoolCondition(DOMNode *node, CAutoResponderParserContext *context)
{
	int processResult=1;
	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);

		if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;
		
		char *nodeNameTmp=CUtil::wc2c(childNode->getNodeName());	
		char *nodeName=(char *)malloc(MAX_STRING_LEN);
		int i,len;
		memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
		free(nodeNameTmp);
		for (i=0,len=strlen(nodeName);i<len;i++)
			nodeName[i]=tolower(nodeName[i]);
		
		int hit=0;
		
		if (!strcmp(nodeName,"and")) processResult=processResult&&processNodeBoolAnd(childNode,context),hit=1;
		if (!strcmp(nodeName,"channel")) processResult=processResult&&processNodeBoolChannel(childNode,context),hit=1;
		if (!strcmp(nodeName,"or")) processResult=processResult&&processNodeBoolOr(childNode,context),hit=1;
		if (!strcmp(nodeName,"playername")) processResult=processResult&&processNodeBoolPlayername(childNode,context),hit=1;
		if (!strcmp(nodeName,"message")) processResult=processResult&&processNodeBoolMessage(childNode,context),hit=1;
		if (!strcmp(nodeName,"testvariable")) processResult=processResult&&processNodeBoolTestvariable(childNode,context),hit=1;
		if (!strcmp(nodeName,"not")) processResult=processResult&&processNodeBoolNot(childNode,context),hit=1;

		free(nodeName);
		if (!hit)
		{
			breakProcessing(node);
			return 0;
		}	
	}
	char *buf=(char *)malloc(MAX_STRING_LEN);
	sprintf(buf,"DEBUG: 'condition' block return with result: %s",processResult?"true":"false");
	registerDebug(buf,context);
	free(buf);
	return processResult;
}

int CAutoResponderParser::processNodeBoolAnd(DOMNode *node, CAutoResponderParserContext *context)
{
	int processResult=1;
	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);

		if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;
		
		char *nodeNameTmp=CUtil::wc2c(childNode->getNodeName());	
		char *nodeName=(char *)malloc(MAX_STRING_LEN);
		int i,len;
		memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
		free(nodeNameTmp);
		for (i=0,len=strlen(nodeName);i<len;i++)
			nodeName[i]=tolower(nodeName[i]);
		
		int hit=0;
		
		if (!strcmp(nodeName,"and")) processResult=processResult&&processNodeBoolAnd(childNode,context),hit=1;
		if (!strcmp(nodeName,"channel")) processResult=processResult&&processNodeBoolChannel(childNode,context),hit=1;
		if (!strcmp(nodeName,"or")) processResult=processResult&&processNodeBoolOr(childNode,context),hit=1;
		if (!strcmp(nodeName,"playername")) processResult=processResult&&processNodeBoolPlayername(childNode,context),hit=1;
		if (!strcmp(nodeName,"message")) processResult=processResult&&processNodeBoolMessage(childNode,context),hit=1;
		if (!strcmp(nodeName,"testvariable")) processResult=processResult&&processNodeBoolTestvariable(childNode,context),hit=1;
		if (!strcmp(nodeName,"not")) processResult=processResult&&processNodeBoolNot(childNode,context),hit=1;

		free(nodeName);
		if (!hit)
		{
			breakProcessing(node);
			return 0;
		}	
	}	
	char *buf=(char *)malloc(MAX_STRING_LEN);
	sprintf(buf,"DEBUG: 'and' block return with result: %s",processResult?"true":"false");
	registerDebug(buf,context);
	free(buf);
	return processResult;
}

int CAutoResponderParser::processNodeBoolOr(DOMNode *node, CAutoResponderParserContext *context)
{
	int processResult=0;
	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);

		if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;
		
		char *nodeNameTmp=CUtil::wc2c(childNode->getNodeName());	
		char *nodeName=(char *)malloc(MAX_STRING_LEN);
		int i,len;
		memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
		free(nodeNameTmp);
		for (i=0,len=strlen(nodeName);i<len;i++)
			nodeName[i]=tolower(nodeName[i]);
		
		int hit=0;
		
		if (!strcmp(nodeName,"and")) processResult=processResult||processNodeBoolAnd(childNode,context),hit=1;
		if (!strcmp(nodeName,"channel")) processResult=processResult||processNodeBoolChannel(childNode,context),hit=1;
		if (!strcmp(nodeName,"or")) processResult=processResult||processNodeBoolOr(childNode,context),hit=1;
		if (!strcmp(nodeName,"playername")) processResult=processResult||processNodeBoolPlayername(childNode,context),hit=1;
		if (!strcmp(nodeName,"message")) processResult=processResult||processNodeBoolMessage(childNode,context),hit=1;
		if (!strcmp(nodeName,"testvariable")) processResult=processResult||processNodeBoolTestvariable(childNode,context),hit=1;
		if (!strcmp(nodeName,"not")) processResult=processResult||processNodeBoolNot(childNode,context),hit=1;

		free(nodeName);
		if (!hit)
		{
			breakProcessing(node);
			return 0;
		}	
	}
	char *buf=(char *)malloc(MAX_STRING_LEN);
	sprintf(buf,"DEBUG: 'or' block return with result: %s",processResult?"true":"false");
	registerDebug(buf,context);
	free(buf);
	return processResult;
}

int CAutoResponderParser::processNodeBoolChannel(DOMNode *node, CAutoResponderParserContext *context)
{	
	char *buf=(char *)malloc(MAX_STRING_LEN);
	char *matchText=CUtil::getNodeAttribute(node,"match");
	replaceSpecialStrings(matchText,context);	
	if (regexpProxy.match(context->channel,matchText))
	{
		sprintf(buf,"DEBUG: channel match '%s' against '%s': true",context->channel,matchText);
		registerDebug(buf,context);
		free(buf);
		free(matchText);
		return 1;
	} else {
		sprintf(buf,"DEBUG: channel match '%s' against '%s': false",context->channel,matchText);
		registerDebug(buf,context);
		free(buf);
		free(matchText);
		return 0;
	}
}

int CAutoResponderParser::processNodeBoolPlayername(DOMNode *node, CAutoResponderParserContext *context)
{		
	char *buf=(char *)malloc(MAX_STRING_LEN);
	char *matchText=CUtil::getNodeAttribute(node,"match");	
	replaceSpecialStrings(matchText,context);
	if (regexpProxy.match(context->playerName,matchText))
	{
		sprintf(buf,"DEBUG: message match '%s' against '%s': true",context->playerName,matchText);
		registerDebug(buf,context);
		free(buf);
		free(matchText);
		return 1;
	} else {
		sprintf(buf,"DEBUG: playername match '%s' against '%s': false",context->playerName,matchText);
		registerDebug(buf,context);
		free(buf);
		free(matchText);
		return 0;
	}
}

int CAutoResponderParser::processNodeBoolTestvariable(DOMNode *node, CAutoResponderParserContext *context)
{
	char *buf=(char *)malloc(MAX_STRING_LEN);
	char *nameText=CUtil::getNodeAttribute(node,"value");	
	char *valueText=CUtil::getNodeAttribute(node,"name");
	replaceSpecialStrings(nameText,context);
	replaceSpecialStrings(valueText,context);
	
	
	if (!strcmp(valueText,context->getVariable(nameText)))
	{ 
		sprintf(buf,"DEBUG: textvariable name='%s' value='%s' actualvalue='%s': true",nameText,valueText,context->getVariable(nameText));
		registerDebug(buf,context);
		free(buf);
		free(nameText);
		free(valueText);
		return 1;
	} else {
		sprintf(buf,"DEBUG: textvariable name='%s' value='%s' actualvalue='%s': false",nameText,valueText,context->getVariable(nameText));
		registerDebug(buf,context);
		free(buf);
		free(nameText);
		free(valueText);
		return 0;
	}
	
}

void CAutoResponderParser::processAction(DOMNode *node, CAutoResponderParserContext *context)
{
	if (node->getNodeType()!=DOMNode::ELEMENT_NODE)
		return;
	registerDebug("DEBUG: processing abstract action",context);
	int hit=0;
	char *nodeNameTmp=CUtil::wc2c(node->getNodeName());	
	char *nodeName=(char *)malloc(MAX_STRING_LEN);
	int i,len;
	memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
	free(nodeNameTmp);
	for (i=0,len=strlen(nodeName);i<len;i++)
		nodeName[i]=tolower(nodeName[i]);
	
	if (!strcmp(nodeName,"dance")) processNodeDance(node,context),hit=1;
	if (!strcmp(nodeName,"if")) processNodeIf(node,context),hit=1;
	if (!strcmp(nodeName,"ignoreplayer")) processNodeIgnoreplayer(node,context),hit=1;
	if (!strcmp(nodeName,"random")) processNodeRandom(node,context),hit=1;
	if (!strcmp(nodeName,"repeat")) processNodeRepeat(node,context),hit=1;
	if (!strcmp(nodeName,"sequence")) processNodeSequence(node,context),hit=1;
	if (!strcmp(nodeName,"say")) processNodeSay(node,context),hit=1;
	if (!strcmp(nodeName,"setvariable")) processNodeSetvariable(node,context),hit=1;
	if (!strcmp(nodeName,"sleep")) processNodeSleep(node,context),hit=1;
	
	free(nodeName);
	if (!hit)
	{		
		breakProcessing(node);		
		return;
	}	
}

void CAutoResponderParser::breakProcessing(DOMNode *node)
{
	char *nodeNameTmp=CUtil::wc2c(node->getNodeName());	
	char *nodeName=(char *)malloc(MAX_STRING_LEN);
	int i,len;
	memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
	free(nodeNameTmp);
	for (i=0,len=strlen(nodeName);i<len;i++)
		nodeName[i]=tolower(nodeName[i]);

	char *buf=(char *)malloc(MAX_STRING_LEN);
	sprintf(buf,"Responder parse error: node '%s' is unknown!",nodeName);
	
	AfxMessageBox(buf);
	free(buf);
	free(nodeName);
}

int CAutoResponderParser::processNodeBoolMessage(DOMNode *node, CAutoResponderParserContext *context)
{
	char *buf=(char *)malloc(MAX_STRING_LEN);
	char *matchText=CUtil::getNodeAttribute(node,"match");	
	replaceSpecialStrings(matchText,context);	
	if (regexpProxy.match(context->message,matchText))
	{
		sprintf(buf,"DEBUG: message match '%s' against '%s': true",context->message,matchText);
		registerDebug(buf,context);		
		free(buf);
		free(matchText);
		return 1;
	} else {
		sprintf(buf,"DEBUG: message match '%s' against '%s': false",context->message,matchText);
		registerDebug(buf,context);
		free(buf);
		free(matchText);
		return 0;
	}
}


void CAutoResponderParser::registerDebug(char *msg,CAutoResponderParserContext *context)
{
	if (context->actionLog)
	{
		context->actionLog->InsertItem(0,msg);		
		if (context->actionLog->GetItemCount()>500)
			context->actionLog->DeleteItem(500);
	}
}

void CAutoResponderParser::replaceSpecialStrings(char *msg, CAutoResponderParserContext *context)
{
	char *bufStart=(char *)malloc(MAX_STRING_LEN);
	char *bufEnd=(char *)malloc(MAX_STRING_LEN);


	
	if (strstr(msg,"$$"))
	{
		strcpy(bufStart,msg);
		bufStart[strstr(msg,"$p")-msg]=0;
		strcpy(bufEnd,strstr(msg,"$p")+2);
		sprintf(msg,"%s%s%s",bufStart,"$",bufEnd);
		replaceSpecialStrings(msg,context);
	}
	if (strstr(msg,"$p"))
	{
		strcpy(bufStart,msg);
		bufStart[strstr(msg,"$p")-msg]=0;
		strcpy(bufEnd,strstr(msg,"$p")+2);
		sprintf(msg,"%s%s%s",bufStart,context->playerName,bufEnd);
		replaceSpecialStrings(msg,context);
	}
	if (strstr(msg,"$s"))
	{
		CMemReaderProxy reader;
		CTibiaCharacter *self = reader.readSelfCharacter();
		strcpy(bufStart,msg);
		bufStart[strstr(msg,"$s")-msg]=0;
		strcpy(bufEnd,strstr(msg,"$s")+2);		
		sprintf(msg,"%s%s%s",bufStart,self->name,bufEnd);
		replaceSpecialStrings(msg,context);
		delete self;
	}
	
	if (strstr(msg,"$m"))
	{
		int i,len;
		char msgToPaste[1024];
		memset(msgToPaste,0,1024);
		memcpy(msgToPaste,context->message,200);
		for (i=0,len=strlen(msgToPaste);i<len;i++)
		{
			if (msgToPaste[i]=='$')
				msgToPaste[i]='#';
		}

		strcpy(bufStart,msg);		
		bufStart[strstr(msg,"$m")-msg]=0;
		strcpy(bufEnd,strstr(msg,"$m")+2);		
		sprintf(msg,"%s%s%s",bufStart,msgToPaste,bufEnd);		
		replaceSpecialStrings(msg,context);
		
	}
	free(bufStart);
	free(bufEnd);
	
	
	
}

void CAutoResponderParser::registerWarn(char *msg, CAutoResponderParserContext *context)
{
	if (context->actionLog)
	{
		context->actionLog->InsertItem(0,msg);		
		if (context->actionLog->GetItemCount()>500)
			context->actionLog->DeleteItem(500);
	}
}

int CAutoResponderParser::processNodeBoolNot(DOMNode *node, CAutoResponderParserContext *context)
{
	int processResult=1;
	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);

		if (childNode->getNodeType()!=DOMNode::ELEMENT_NODE)
			continue;
		
		char *nodeNameTmp=CUtil::wc2c(childNode->getNodeName());	
		char *nodeName=(char *)malloc(MAX_STRING_LEN);
		int i,len;
		memcpy(nodeName,nodeNameTmp,strlen(nodeNameTmp)+1);
		free(nodeNameTmp);
		for (i=0,len=strlen(nodeName);i<len;i++)
			nodeName[i]=tolower(nodeName[i]);
		
		int hit=0;
		
		if (!strcmp(nodeName,"and")) processResult=processResult&&processNodeBoolAnd(childNode,context),hit=1;
		if (!strcmp(nodeName,"channel")) processResult=processResult&&processNodeBoolChannel(childNode,context),hit=1;
		if (!strcmp(nodeName,"or")) processResult=processResult&&processNodeBoolOr(childNode,context),hit=1;
		if (!strcmp(nodeName,"playername")) processResult=processResult&&processNodeBoolPlayername(childNode,context),hit=1;
		if (!strcmp(nodeName,"message")) processResult=processResult&&processNodeBoolMessage(childNode,context),hit=1;
		if (!strcmp(nodeName,"testvariable")) processResult=processResult&&processNodeBoolTestvariable(childNode,context),hit=1;
		if (!strcmp(nodeName,"not")) processResult=processResult&&processNodeBoolNot(childNode,context),hit=1;

		free(nodeName);
		if (!hit)
		{
			breakProcessing(node);
			return 1;
		}	
	}	
	char *buf=(char *)malloc(MAX_STRING_LEN);
	sprintf(buf,"DEBUG: 'not' block return with result: %s",processResult?"true":"false");
	registerDebug(buf,context);
	free(buf);
	return !processResult;
}

void CAutoResponderParser::processNodeSequence(DOMNode *node, CAutoResponderParserContext *context)
{
	registerDebug("DEBUG: action: sequence",context);				
	int nodeNr;
	for (nodeNr=0;nodeNr<node->getChildNodes()->getLength()&&!autoResponderStop;nodeNr++)
	{
		DOMNode *childNode = node->getChildNodes()->item(nodeNr);			
		
		processAction(childNode,context);		
	}
	
}



