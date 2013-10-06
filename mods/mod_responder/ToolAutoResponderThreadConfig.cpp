// ToolAutoResponderThreadConfig.cpp: implementation of the CToolAutoResponderThreadConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_responder.h"
#include "ToolAutoResponderThreadConfig.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolAutoResponderThreadConfig::CToolAutoResponderThreadConfig()
{
	status = CToolAutoResponderThreadStatus_notRunning;
}

CToolAutoResponderThreadConfig::~CToolAutoResponderThreadConfig()
{

}
