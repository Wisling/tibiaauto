// AutoResponderParserContext.h: interface for the CAutoResponderParserContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTORESPONDERPARSERCONTEXT_H__492CF6EC_7EC5_48D6_AFA2_D106D799D43C__INCLUDED_)
#define AFX_AUTORESPONDERPARSERCONTEXT_H__492CF6EC_7EC5_48D6_AFA2_D106D799D43C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_STRING_LEN 65536
#define MAX_MESSAGE_LEN 65536

class CAutoResponderParserContext
{
public:
	int isPlayerIgnored(char *name);
	char * getVariable(char *name);
	void setVariable(char *name, char *value);
	void setIgnoredPlayer(char *name,time_t deadline);
	CAutoResponderParserContext(CListCtrl *actionLog, int localEcho);
	virtual ~CAutoResponderParserContext();
	char channel[MAX_STRING_LEN];
	char playerName[MAX_STRING_LEN];
	char message[MAX_MESSAGE_LEN];


	CListCtrl *actionLog;
	int localEcho;

private:
	char **ignorePlayers;
	time_t *ignorePlayersDeadline;
	int ignorePlayersCount;
	int ignorePlayersSize;

	char **variableNames;
	char **variableValues;
	int variableCount;
	int variableSize;

};

#endif // !defined(AFX_AUTORESPONDERPARSERCONTEXT_H__492CF6EC_7EC5_48D6_AFA2_D106D799D43C__INCLUDED_)
