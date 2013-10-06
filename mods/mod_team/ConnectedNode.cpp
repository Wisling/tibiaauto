// ConnectedNode.cpp: implementation of the CConnectedNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_team.h"
#include "ConnectedNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectedNode::CConnectedNode()
{
	connected=0;
	charName[0]=0;
	lastMessageTm=0;
}

CConnectedNode::~CConnectedNode()
{

}
