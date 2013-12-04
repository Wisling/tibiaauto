// ConnectedNodes.h: interface for the CConnectedNodes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTEDNODES_H__EF770DB0_E2A7_4506_95CE_11D37EB668E6__INCLUDED_)
#define AFX_CONNECTEDNODES_H__EF770DB0_E2A7_4506_95CE_11D37EB668E6__INCLUDED_

#define MAX_CONNECTED_NODES 128

#include "ConnectedNode.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConnectedNodes
{
public:
	void clearDisconnectedNodes();
	int shouldAcceptMessage(char *charName);
	CConnectedNode * getNodeByNr(int nr);
	int getMaxNodeCount();
	void refreshNodeInfo(char *charName,int hp, int mana, int maxHp, int maxMana, int x, int y, int z, int isSlave, int distance);
	int isConnected();
	void disconnect();
	int isCharConnected(char *charName);
	void setMasterNode(char *charName);
	char *getMasterNode();
	void findNewMasterNode();
	CConnectedNodes();
	virtual ~CConnectedNodes();
private:
	int findNodeForChar(char *charName);
	char currentMasterNode[33];
	CConnectedNode connectedNodes[MAX_CONNECTED_NODES];

};

#endif // !defined(AFX_CONNECTEDNODES_H__EF770DB0_E2A7_4506_95CE_11D37EB668E6__INCLUDED_)
