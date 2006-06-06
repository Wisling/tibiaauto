// ConnectedNode.h: interface for the CConnectedNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTEDNODE_H__489748D1_6F01_4898_855F_B20056B25768__INCLUDED_)
#define AFX_CONNECTEDNODE_H__489748D1_6F01_4898_855F_B20056B25768__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConnectedNode  
{
public:
	char charName[33];
	int x;
	int y;
	int z;
	int hp;
	int maxHp;
	int mana;
	int maxMana;
	int connected;
	int isSlave;
	int lastMessageTm;
	int distance;

	CConnectedNode();
	virtual ~CConnectedNode();

};

#endif // !defined(AFX_CONNECTEDNODE_H__489748D1_6F01_4898_855F_B20056B25768__INCLUDED_)
