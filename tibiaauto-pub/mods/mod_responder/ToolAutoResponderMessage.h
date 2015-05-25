// CToolAutoResponderMessage.h: interface for the ToolAutoResponderMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLAUTORESPONDERMESSAGE_H__FD207894_E336_451B_A504_DB3B6F0DE9EB__INCLUDED_)
#define AFX_TOOLAUTORESPONDERMESSAGE_H__FD207894_E336_451B_A504_DB3B6F0DE9EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CToolAutoResponderMessage
{
public:
	CToolAutoResponderMessage(char chan[16384], char mess[16384], char nick[16384]);
	CToolAutoResponderMessage();
	virtual ~CToolAutoResponderMessage();
	char nick[16384];
	char chan[16384];
	char mess[16384];
};

#endif // !defined(AFX_TOOLAUTORESPONDERMESSAGE_H__FD207894_E336_451B_A504_DB3B6F0DE9EB__INCLUDED_)
