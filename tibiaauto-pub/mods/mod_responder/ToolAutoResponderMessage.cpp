// ToolAutoResponderMessage.cpp: implementation of the ToolAutoResponderMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_responder.h"
#include "ToolAutoResponderMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif // ifdef _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolAutoResponderMessage::CToolAutoResponderMessage()
{
}

CToolAutoResponderMessage::~CToolAutoResponderMessage()
{
}

CToolAutoResponderMessage::CToolAutoResponderMessage(char chan[], char mess[], char nick[])
{
	memcpy(this->chan, chan, 16384);
	memcpy(this->mess, mess, 16384);
	memcpy(this->nick, nick, 16384);
}
