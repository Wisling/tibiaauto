// HealList.cpp : implementation file
//

#include "stdafx.h"
#include "mod_spellcaster.h"
#include "HealList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHealList dialog


CHealList::CHealList(std::vector<Player> healListParam, CWnd* pParent /*=NULL*/)
	: MyDialog(CHealList::IDD, pParent)
{
	healList = healListParam;
	//{{AFX_DATA_INIT(CHealList)
	//}}AFX_DATA_INIT
}


void CHealList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHealList)
	DDX_Control(pDX, IDC_HEALLIST_LIST, m_list);
	DDX_Control(pDX, IDC_HEALLIST_TRIGGER_HP, m_triggerHP);
	DDX_Control(pDX, IDC_HEALLIST_FRAME, m_healListFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_HEALLIST_NAME, m_name);
	DDX_Control(pDX, IDC_HEALLIST_MAX_HP, m_maxHP);
	DDX_Control(pDX, IDC_HEALLIST_DELETE, m_delete);
	DDX_Control(pDX, IDC_HEALLIST_ADD, m_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHealList, CDialog)
//{{AFX_MSG_MAP(CHealList)
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_HEALLIST_ADD, OnHealListAdd)
ON_BN_CLICKED(IDC_HEALLIST_DELETE, OnHealListDelete)
ON_BN_DOUBLECLICKED(IDC_HEALLIST_DELETE, OnHealListDelete)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHealList message handlers

void CHealList::OnHealListAdd()
{
	char lpNameBuffer[64];
	char lpMaxHPBuffer[8];
	char lpTriggerHPBuffer[8];
	int nameLen      = m_name.GetWindowText(lpNameBuffer, 64);
	int hpLen        = m_maxHP.GetWindowText(lpMaxHPBuffer, 8);
	int triggerHPLen = m_triggerHP.GetWindowText(lpTriggerHPBuffer, 8);
	if (nameLen && hpLen && triggerHPLen)
	{
		m_list.InsertItem(healList.size(), "");
		m_list.SetItemText(0, 0, lpNameBuffer);
		m_list.SetItemText(0, 1, lpMaxHPBuffer);
		m_list.SetItemText(0, 2, lpTriggerHPBuffer);
		m_name.SetWindowText("");
		m_maxHP.SetWindowText("");
		m_triggerHP.SetWindowText("");
		Player temp;
		temp.randTriggerHP = 0;
		strcpy(temp.name, lpNameBuffer);
		temp.maxHP     = atoi(lpMaxHPBuffer);
		temp.triggerHP = atoi(lpTriggerHPBuffer);
		healList.push_back(temp);
	}
	else
	{
		AfxMessageBox("Some entries are blank, please try again!");
	}
}

void CHealList::OnHealListDelete()
{
	int nSelRows = m_list.GetSelectedCount();
	if(!nSelRows)
		return;

	int* pnArrayOfSelRows = new int[nSelRows];
	if(!pnArrayOfSelRows)
		return;

	int nTemp    = nSelRows;
	POSITION pos = m_list.GetFirstSelectedItemPosition();

	int i = m_list.GetNextSelectedItem(pos);

	while (i != -1)
	{
		int find = i;
		nTemp                   = nTemp - 1;
		pnArrayOfSelRows[nTemp] = i;
		i                       = m_list.GetNextSelectedItem(pos);
		for (size_t k = 0; k < healList.size(); k++)
		{
			if(healList[k].name == m_list.GetItemText(find, 0))
				healList.erase(healList.begin() + k);
		}
	}

	for(int j = 0; j < nSelRows; j++)
		m_list.DeleteItem(pnArrayOfSelRows[j]);

	delete(pnArrayOfSelRows);
	pnArrayOfSelRows = NULL;
}

void CHealList::Mem2List()
{
	char lpBuffer[32];
	for (size_t i = 0; i < healList.size(); i++)
	{
		Player temp = healList[i];
		m_list.InsertItem(i, "");
		m_list.SetItemText(0, 0, temp.name);
		sprintf(lpBuffer, "%d", temp.maxHP);
		m_list.SetItemText(0, 1, lpBuffer);
		sprintf(lpBuffer, "%d", temp.triggerHP);
		m_list.SetItemText(0, 2, lpBuffer);
	}
}

BOOL CHealList::OnInitDialog() {
	CDialog::OnInitDialog();
	skin.SetButtonSkin(     m_OK);
	skin.SetButtonSkin(     m_delete);
	skin.SetButtonSkin(     m_add);

	m_list.InsertColumn(0, "Name", LVCFMT_LEFT, 66);
	m_list.InsertColumn(1, "Max HP", LVCFMT_LEFT, 50);
	m_list.InsertColumn(2, "Heal At", LVCFMT_LEFT, 48);

	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	Mem2List();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
