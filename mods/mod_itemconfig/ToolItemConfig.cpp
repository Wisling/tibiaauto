// ToolItemConfig.cpp : implementation file
//

#include "stdafx.h"
#include "mod_itemconfig.h"
#include "ToolItemConfig.h"
#include "TibiaItem.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig dialog


CToolItemConfig::CToolItemConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CToolItemConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolItemConfig)
	//}}AFX_DATA_INIT
}


void CToolItemConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolItemConfig)
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_LOOTED_LIST, m_lootedList);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_ITEMS_LIST, m_itemsList);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_FOODLIST, m_foodList);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG_CORPSES_LIST, m_corpsesList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolItemConfig, CDialog)
	//{{AFX_MSG_MAP(CToolItemConfig)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TOOL_ITEMCONFIG_REFRESH, OnToolItemconfigRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig message handlers

void CToolItemConfig::OnOK() 
{
	ShowWindow(SW_HIDE);
}

void CToolItemConfig::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void CToolItemConfig::OnToolItemconfigRefresh() 
{
	int i;
	char buf[16384];
	CTibiaItemProxy itemProxy;
	itemProxy.refreshItemLists();
	

	while (m_itemsList.GetCount()) m_itemsList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsItemsCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsItemsId(i),itemProxy.getItemsItems(i));
		m_itemsList.AddString(buf);
	}

	while (m_foodList.GetCount()) m_foodList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsFoodCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsFoodId(i),itemProxy.getItemsFood(i));
		m_foodList.AddString(buf);
	}

	while (m_corpsesList.GetCount()) m_corpsesList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsCorpsesCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsCorpsesId(i),itemProxy.getItemsCorpses(i));
		m_corpsesList.AddString(buf);
	}

	while (m_lootedList.GetCount()) m_lootedList.DeleteString(0);
	for (i=0;i<itemProxy.getItemsLootedCount();i++)
	{
		sprintf(buf,"(0x%x) %s",itemProxy.getItemsLootedId(i),itemProxy.getItemsLooted(i));
		m_lootedList.AddString(buf);
	}


}

BOOL CToolItemConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnToolItemconfigRefresh();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
