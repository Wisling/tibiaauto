// HotkeyManager.h: interface for the CHotkeyManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEHANDLER_H__CB9257F9_5D0D_4DC8_A04F_6B9AF26445BE__INCLUDED_)
#define AFX_MESSAGEHANDLER_H__CB9257F9_5D0D_4DC8_A04F_6B9AF26445BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMessageHandler  
{
	int messageInPipe;
	int messageOutPipe;
	int getNextMessage(char*, int*);
public:
	CMessageHandler(int a_messageInPipe, int a_messageOutPipe);
	virtual ~CMessageHandler();

};

class CHotkeyManager
{
public:

};

class CRecvActionMessageManager: private CMessageHandler
{
public:


};

class CSentActionMessageManager
{
public:

};
#endif // !defined(AFX_MESSAGEHANDLER_H__CB9257F9_5D0D_4DC8_A04F_6B9AF26445BE__INCLUDED_)
