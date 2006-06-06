// ConnectedNodes.cpp: implementation of the CConnectedNodes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mod_team.h"
#include "ConnectedNodes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectedNodes::CConnectedNodes()
{
	currentMasterNode[0]='\0';
}

CConnectedNodes::~CConnectedNodes()
{

}

void CConnectedNodes::findNewMasterNode()
{
	int i;
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{
		if (connectedNodes[i].connected)
		{
			strcpy(currentMasterNode,connectedNodes[i].charName);
			return;
		}
	}
	currentMasterNode[0]='\0';
}

char *CConnectedNodes::getMasterNode()
{
	return currentMasterNode;
}

void CConnectedNodes::setMasterNode(char *charName)
{
	if (strlen(charName)>32) charName[32]='\0';
	strcpy(currentMasterNode,charName);
}

int CConnectedNodes::isCharConnected(char *charName)
{	
	int i;	
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{
		if (!strcmp(connectedNodes[i].charName,charName)&&connectedNodes[i].connected)
			return 1;
	}
	return 0;
}

void CConnectedNodes::disconnect()
{
	int i;
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{
		connectedNodes[i].connected=0;
	}
	currentMasterNode[0]=0;
}

int CConnectedNodes::isConnected()
{
	// isConnected iff is connected to at least one node
	int i;
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{
		if (connectedNodes[i].connected)
			return 1;
	}
	return 0;
}

void CConnectedNodes::refreshNodeInfo(char *charName, int hp, int mana, int maxHp, int maxMana, int x, int y, int z, int isSlave, int distance)
{
	int nodeNr=findNodeForChar(charName);	

	if (connectedNodes[nodeNr].connected&&connectedNodes[nodeNr].distance<distance)
	{
		// never accept update from a more distant node (to avoid information loops)
		return;
	}

	connectedNodes[nodeNr].hp=hp;
	connectedNodes[nodeNr].mana=mana;
	connectedNodes[nodeNr].maxHp=maxHp;
	connectedNodes[nodeNr].maxMana=maxMana;
	connectedNodes[nodeNr].x=x;
	connectedNodes[nodeNr].y=y;
	connectedNodes[nodeNr].z=z;
	if (isSlave!=-1)
	{
		connectedNodes[nodeNr].isSlave=isSlave;
	}

	connectedNodes[nodeNr].lastMessageTm=GetTickCount();
	connectedNodes[nodeNr].connected=1;
	connectedNodes[nodeNr].distance=distance;
}

int CConnectedNodes::findNodeForChar(char *charName)
{
	int i;
	// when the charName is already connected
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{
		if (!strcmp(connectedNodes[i].charName,charName)&&connectedNodes[i].connected)
		{
			return i;
		}
	}
	// when the charname is not connected - find first empty slot)
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{
		if (!connectedNodes[i].connected)
		{
			strcpy(connectedNodes[i].charName,charName);
			connectedNodes[i].connected=0;
			connectedNodes[i].isSlave=0;
			return i;
		}
	}
}

int CConnectedNodes::getMaxNodeCount()
{
	return MAX_CONNECTED_NODES;
}

CConnectedNode * CConnectedNodes::getNodeByNr(int nr)
{
	if (nr>=MAX_CONNECTED_NODES) return NULL;
	return &connectedNodes[nr];
}

int CConnectedNodes::shouldAcceptMessage(char *charName)
{
	char lowerCharName[33];
	char lowerCompareNode[33];
	int i;	
	for (i=0;i<33;i++) 
	{
		lowerCharName[i]=tolower(charName[i]);
		lowerCompareNode[i]=tolower(currentMasterNode[i]);
	}
	if (!strcmp(lowerCharName,lowerCompareNode)) return 1;
	
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{		
		if (connectedNodes[i].connected&&connectedNodes[i].isSlave)
		{
			int len=strlen(connectedNodes[i].charName);
			int j;
			for (j=0;j<len;j++)
			{				
				lowerCompareNode[j]=tolower(connectedNodes[i].charName[j]);
			}
			if (!strcmp(lowerCharName,lowerCompareNode)) return 1;			
		}
	}
	return 0;
}

void CConnectedNodes::clearDisconnectedNodes()
{
	int i;
	for (i=0;i<MAX_CONNECTED_NODES;i++)
	{
		if (connectedNodes[i].connected&&GetTickCount()-connectedNodes[i].lastMessageTm>30000)
		{
			// if no message about node for 30s - then the node is offline
			connectedNodes[i].connected=0;
		}
	}
}
