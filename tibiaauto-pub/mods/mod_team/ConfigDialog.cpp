// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_team.h"
#include "ConfigDialog.h"
#include <MemReader.h>
#include "ConnectedNode.h"
#include "ConnectedNodes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

extern CConnectedNodes connectedNodes;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_teamApp *app, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app = app;
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_SLAVELIST, m_slaveList);
	DDX_Control(pDX, IDC_MASTER_CHAR, m_masterChar);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable()
{
	if (m_enable.GetCheck())
	{
		m_app->controlsToConfig();
		if (m_app->validateConfig(1))
			m_app->start();
		else
			m_enable.SetCheck(0);
	}
	else
	{
		m_app->stop();
	}
}

void CConfigDialog::disableControls()
{
}

void CConfigDialog::enableControls()
{
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	sprintf(buf, "%s", configData->masterChar);
	m_masterChar.SetWindowText(buf);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	//char buf[128];
	CConfigData *newConfigData = new CConfigData();
	m_masterChar.GetWindowText(newConfigData->masterChar, 127);


	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1001)
	{
		CMemReader& reader = CMemReader::getMemReader();
		CTibiaCharacter self;
		reader.readSelfCharacter(&self);
		int i;
		KillTimer(1001);

		int count = m_slaveList.GetItemCount();

		int refreshedNodes[1000];
		memset(refreshedNodes, 0x0, sizeof(int) * 1000);


		for (i = 0; i < count; i++)
		{
			int nodeNr = m_slaveList.GetItemData(i);
			refreshedNodes[nodeNr] = 1;
			if (connectedNodes.getNodeByNr(nodeNr)->connected)
			{
				char buf[256];
				CConnectedNode *connectedNode = connectedNodes.getNodeByNr(nodeNr);
				m_slaveList.SetItemText(i, 0, connectedNode->charName);
				sprintf(buf, "(%d,%d,%d)", self.x - connectedNode->x, self.y - connectedNode->y, self.z - connectedNode->z);
				m_slaveList.SetItemText(i, 2, buf);
				sprintf(buf, "%d/%d", connectedNode->hp, connectedNode->maxHp);
				m_slaveList.SetItemText(i, 3, buf);
				sprintf(buf, "%d/%d", connectedNode->mana, connectedNode->maxMana);
				m_slaveList.SetItemText(i, 4, buf);
				sprintf(buf, "%d", GetTickCount() - connectedNode->lastMessageTm);
				m_slaveList.SetItemText(i, 5, buf);
				sprintf(buf, "%s", connectedNode->isSlave ? "yes" : "no");
				m_slaveList.SetItemText(i, 6, buf);
				sprintf(buf, "%d", connectedNode->distance);
				m_slaveList.SetItemText(i, 7, buf);
			}
			else
			{
				m_slaveList.DeleteItem(i);
				count--;
			}
		}

		for (i = 0; i < connectedNodes.getMaxNodeCount(); i++)
		{
			CConnectedNode *connectedNode = connectedNodes.getNodeByNr(i);
			if (connectedNode->connected && !refreshedNodes[i])
			{
				char buf[256];
				m_slaveList.InsertItem(0, connectedNode->charName);
				sprintf(buf, "(%d,%d,%d)", self.x - connectedNode->x, self.y - connectedNode->y, self.z - connectedNode->z);
				m_slaveList.SetItemText(0, 2, buf);
				sprintf(buf, "%d/%d", connectedNode->hp, connectedNode->maxHp);
				m_slaveList.SetItemText(0, 3, buf);
				sprintf(buf, "%d/%d", connectedNode->mana, connectedNode->maxMana);
				m_slaveList.SetItemText(0, 4, buf);
				sprintf(buf, "%d", GetTickCount() - connectedNode->lastMessageTm);
				m_slaveList.SetItemText(0, 5, buf);
				sprintf(buf, "%s", connectedNode->isSlave ? "yes" : "no");
				m_slaveList.SetItemText(0, 6, buf);
				sprintf(buf, "%d", connectedNode->distance);
				m_slaveList.SetItemText(0, 7, buf);

				m_slaveList.SetItemData(0, i);
			}
		}


		SetTimer(1001, 500, NULL);
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_slaveList.InsertColumn(0, "Player name", LVCFMT_LEFT, 100);
	m_slaveList.InsertColumn(1, "Last message [ms]", LVCFMT_LEFT, 50);
	m_slaveList.InsertColumn(2, "Distance", LVCFMT_LEFT, 80);
	m_slaveList.InsertColumn(3, "HP", LVCFMT_LEFT, 80);
	m_slaveList.InsertColumn(4, "Mana", LVCFMT_LEFT, 80);
	m_slaveList.InsertColumn(5, "Ping", LVCFMT_LEFT, 50);
	m_slaveList.InsertColumn(6, "Slave", LVCFMT_LEFT, 30);
	m_slaveList.InsertColumn(7, "Dist", LVCFMT_LEFT, 30);

	m_slaveList.SetExtendedStyle(m_slaveList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	SetTimer(1001, 500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}
