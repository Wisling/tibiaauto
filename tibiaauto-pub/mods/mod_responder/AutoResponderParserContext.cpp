// AutoResponderParserContext.cpp: implementation of the CAutoResponderParserContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_responder.h"
#include "AutoResponderParserContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoResponderParserContext::CAutoResponderParserContext(CListCtrl *actionLog, int localEcho)
{
	memset(channel, 0, 1024);
	memset(playerName, 0, 1024);
	memset(message, 0, 1024);

	variableCount  = 0;
	variableSize   = 0;
	variableNames  = NULL;
	variableValues = NULL;

	ignorePlayersCount    = 0;
	ignorePlayersSize     = 0;
	ignorePlayers         = NULL;
	ignorePlayersDeadline = NULL;

	this->actionLog = actionLog;
	this->localEcho = localEcho;
}

CAutoResponderParserContext::~CAutoResponderParserContext()
{
}

void CAutoResponderParserContext::setIgnoredPlayer(char *name, time_t deadline)
{
	int i;
	for (i = 0; i < ignorePlayersCount; i++)
	{
		if (!strcmp(ignorePlayers[i], name))
		{
			// ignored player found - check whether to extend ignore period
			if (ignorePlayersDeadline[i] < deadline)
				ignorePlayersDeadline[i] = deadline;
			return;
		}
	}
	if (ignorePlayersCount >= ignorePlayersSize - 2)
	{
		ignorePlayersSize     = ignorePlayersSize * 2 + 2;
		ignorePlayers         = (char **)realloc(ignorePlayers, ignorePlayersSize * sizeof(char *));
		ignorePlayersDeadline = (time_t *)realloc(ignorePlayersDeadline, ignorePlayersSize * sizeof(time_t));
	}
	ignorePlayers[ignorePlayersCount] = (char *)malloc(sizeof(char) * MAX_STRING_LEN);
	sprintf(ignorePlayers[ignorePlayersCount], "%s", name);
	ignorePlayersDeadline[ignorePlayersCount] = deadline;
	ignorePlayersCount++;
}

void CAutoResponderParserContext::setVariable(char *name, char *value)
{
	int i;
	for (i = 0; i < variableCount; i++)
	{
		if (!strcmp(variableNames[i], name))
		{
			strcpy(variableValues[i], value);
			return;
		}
	}
	if (variableCount >= variableSize - 2)
	{
		variableSize   = variableSize * 2 + 2;
		variableNames  = (char **)realloc(variableNames, variableSize * sizeof(char *));
		variableValues = (char **)realloc(variableValues, variableSize * sizeof(char *));
	}
	variableNames[variableCount]  = (char *)malloc(sizeof(char) * MAX_STRING_LEN);
	variableValues[variableCount] = (char *)malloc(sizeof(char) * MAX_STRING_LEN);
	strcpy(variableNames[variableCount], name);
	strcpy(variableValues[variableCount], value);
	variableCount++;
}

char * CAutoResponderParserContext::getVariable(char *name)
{
	int i;
	for (i = 0; i < variableCount; i++)
	{
		if (!strcmp(variableNames[i], name))
			return variableValues[i];
	}
	return "";
}

int CAutoResponderParserContext::isPlayerIgnored(char *name)
{
	int i;
	time_t t = time(NULL);
	for (i = 0; i < ignorePlayersCount; i++)
	{
		if (!strcmp(ignorePlayers[i], name) && ignorePlayersDeadline[i] >= t)
			return 1;
	}
	return 0;
}
