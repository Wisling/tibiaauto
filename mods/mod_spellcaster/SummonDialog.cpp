// SummonDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_spellcaster.h"
#include "SummonDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SummonDialog dialog


SummonDialog::SummonDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(SummonDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(SummonDialog)
	//}}AFX_DATA_INIT
}


void SummonDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SummonDialog)
	DDX_Control(pDX, IDC_CREATURE_LIST, m_creatureList);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_NAME, m_summonName);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_MANA, m_summonMana);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON, m_summon);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_SUMMON_LESSTHAN, m_summonLessThan);
	DDX_Control(pDX, IDC_FRAME_SUMMON_CONTROL, m_SummonFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SummonDialog, CDialog)
	//{{AFX_MSG_MAP(SummonDialog)
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER_SUMMON, OnToolSpellcasterSummon)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_CREATURE_LIST, OnItemchangingCreatureList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SummonDialog message handlers

void SummonDialog::OnToolSpellcasterSummon() {
	int val = m_summon.GetCheck();
	m_summonLessThan.EnableWindow(val);
	m_summonMana.EnableWindow(val);
	m_summonName.EnableWindow(val);	
}

void SummonDialog::disableControls() {
	m_summon.EnableWindow(false);
	m_summonLessThan.EnableWindow(false);
	m_summonMana.EnableWindow(false);
	m_summonName.EnableWindow(false);
}

void SummonDialog::enableControls() {
	m_summon.EnableWindow(true);
	if (m_summon.GetCheck()){
		m_summonLessThan.EnableWindow(true);
		m_summonMana.EnableWindow(true);
		m_summonName.EnableWindow(true);
	}
}

void SummonDialog::configToControls(CConfigData *configData) {
	char buf[128];
	m_summon.SetCheck(configData->summon);
	sprintf(buf,"%d",configData->summonLessThan);	m_summonLessThan.SetWindowText(buf);
	sprintf(buf,"%s",configData->summonName);		m_summonName.SetWindowText(buf);	
	sprintf(buf,"%d",configData->summonMana);		m_summonMana.SetWindowText(buf);
	OnToolSpellcasterSummon();
}

void SummonDialog::controlsToConfig(CConfigData *newConfigData) {
	char buf[128];
	newConfigData->summon = m_summon.GetCheck();
	m_summonLessThan.GetWindowText(buf,127);newConfigData->summonLessThan=atoi(buf);
	m_summonMana.GetWindowText(buf,127);newConfigData->summonMana=atoi(buf);
	m_summonName.GetWindowText(newConfigData->summonName,127);
}

BOOL SummonDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int nItem;
	m_LargeImg.Create(32, 32, ILC_COLOR32 | ILC_MASK, 0, 0);

	CBitmap bmp;
	bmp.LoadBitmap(IDB_CREATURES);

	m_LargeImg.Add(&bmp, RGB(255, 0, 255));

	m_creatureList.SetImageList(&m_LargeImg, LVSIL_NORMAL);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 120;
	lvColumn.pszText = "Name";
	m_creatureList.InsertColumn(0, &lvColumn);

	lvColumn.pszText = "Mana";
	m_creatureList.InsertColumn(1, &lvColumn);

	LVITEM lvItem;
	lvItem.mask = LVIF_IMAGE | LVIF_TEXT;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	lvItem.pszText = "Dwarf Guard";
	lvItem.iImage = 0;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(0, 1, "650");

	lvItem.iItem = 1;
	lvItem.pszText = "Fire Elemental";
	lvItem.iImage = 1;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(1, 1, "690");

	lvItem.iItem = 2;
	lvItem.pszText = "Orc Leader";
	lvItem.iImage = 2;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(2, 1, "640");

	lvItem.iItem = 3;
	lvItem.pszText = "Fire Devil";
	lvItem.iImage = 3;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(3, 1, "530");

	lvItem.iItem = 4;
	lvItem.pszText = "Monk";
	lvItem.iImage = 4;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(4, 1, "600");

	lvItem.iItem = 5;
	lvItem.pszText = "Orc Berserker";
	lvItem.iImage = 5;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(5, 1, "590");

	lvItem.iItem = 6;
	lvItem.pszText = "Stone Golem";
	lvItem.iImage = 6;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(6, 1, "590");

	lvItem.iItem = 7;
	lvItem.pszText = "Gozzler";
	lvItem.iImage = 7;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(7, 1, "800");

	lvItem.iItem = 8;
	lvItem.pszText = "Demon Skeleton";
	lvItem.iImage = 8;
	nItem = m_creatureList.InsertItem(&lvItem);
	m_creatureList.SetItemText(8, 1, "620");
	OnToolSpellcasterSummon();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SummonDialog::OnItemchangingCreatureList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
	if((pNMListView->uNewState & LVIS_SELECTED)) 	{
		m_summonName.SetWindowText(m_creatureList.GetItemText(pNMListView->iItem, 0));
		m_summonMana.SetWindowText(m_creatureList.GetItemText(pNMListView->iItem, 1));

		*pResult = 1;
	}
}
