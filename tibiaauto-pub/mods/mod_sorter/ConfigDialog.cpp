// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_sorter.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_sorterApp *app, CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app = app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_BAG8, m_Bag8Frame);
	DDX_Control(pDX, IDC_FRAME_BAG7, m_Bag7Frame);
	DDX_Control(pDX, IDC_FRAME_BAG6, m_Bag6Frame);
	DDX_Control(pDX, IDC_FRAME_BAG5, m_Bag5Frame);
	DDX_Control(pDX, IDC_FRAME_BAG4, m_Bag4Frame);
	DDX_Control(pDX, IDC_FRAME_BAG3, m_Bag3Frame);
	DDX_Control(pDX, IDC_FRAME_BAG2, m_Bag2Frame);
	DDX_Control(pDX, IDC_FRAME_BAG1, m_Bag1Frame);
	DDX_Control(pDX, IDC_SORTER_ITEM_LIST, m_sortItemList);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SORTER_BAG1, m_Bag[0]);
	DDX_Control(pDX, IDC_SORTER_BAG1_IN, m_BagIn[0]);
	DDX_Control(pDX, IDC_SORTER_BAG1_OUT, m_BagOut[0]);
	DDX_Control(pDX, IDC_SORTER_BAG2, m_Bag[1]);
	DDX_Control(pDX, IDC_SORTER_BAG2_IN, m_BagIn[1]);
	DDX_Control(pDX, IDC_SORTER_BAG2_OUT, m_BagOut[1]);
	DDX_Control(pDX, IDC_SORTER_BAG3, m_Bag[2]);
	DDX_Control(pDX, IDC_SORTER_BAG3_IN, m_BagIn[2]);
	DDX_Control(pDX, IDC_SORTER_BAG3_OUT, m_BagOut[2]);
	DDX_Control(pDX, IDC_SORTER_BAG4, m_Bag[3]);
	DDX_Control(pDX, IDC_SORTER_BAG4_IN, m_BagIn[3]);
	DDX_Control(pDX, IDC_SORTER_BAG4_OUT, m_BagOut[3]);
	DDX_Control(pDX, IDC_SORTER_BAG5, m_Bag[4]);
	DDX_Control(pDX, IDC_SORTER_BAG5_IN, m_BagIn[4]);
	DDX_Control(pDX, IDC_SORTER_BAG5_OUT, m_BagOut[4]);
	DDX_Control(pDX, IDC_SORTER_BAG6, m_Bag[5]);
	DDX_Control(pDX, IDC_SORTER_BAG6_IN, m_BagIn[5]);
	DDX_Control(pDX, IDC_SORTER_BAG6_OUT, m_BagOut[5]);
	DDX_Control(pDX, IDC_SORTER_BAG7, m_Bag[6]);
	DDX_Control(pDX, IDC_SORTER_BAG7_IN, m_BagIn[6]);
	DDX_Control(pDX, IDC_SORTER_BAG7_OUT, m_BagOut[6]);
	DDX_Control(pDX, IDC_SORTER_BAG8, m_Bag[7]);
	DDX_Control(pDX, IDC_SORTER_BAG8_IN, m_BagIn[7]);
	DDX_Control(pDX, IDC_SORTER_BAG8_OUT, m_BagOut[7]);
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_SORTER_BAG1_IN, OnSortEntryAdd1)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG1_IN, OnSortEntryAdd1)
ON_BN_CLICKED(IDC_SORTER_BAG1_OUT, OnSortEntryRemove1)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG1_OUT, OnSortEntryRemove1)
ON_BN_CLICKED(IDC_SORTER_BAG2_IN, OnSortEntryAdd2)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG2_IN, OnSortEntryAdd2)
ON_BN_CLICKED(IDC_SORTER_BAG2_OUT, OnSortEntryRemove2)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG2_OUT, OnSortEntryRemove2)
ON_BN_CLICKED(IDC_SORTER_BAG3_IN, OnSortEntryAdd3)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG3_IN, OnSortEntryAdd3)
ON_BN_CLICKED(IDC_SORTER_BAG3_OUT, OnSortEntryRemove3)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG3_OUT, OnSortEntryRemove3)
ON_BN_CLICKED(IDC_SORTER_BAG4_IN, OnSortEntryAdd4)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG4_IN, OnSortEntryAdd4)
ON_BN_CLICKED(IDC_SORTER_BAG4_OUT, OnSortEntryRemove4)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG4_OUT, OnSortEntryRemove4)
ON_BN_CLICKED(IDC_SORTER_BAG5_IN, OnSortEntryAdd5)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG5_IN, OnSortEntryAdd5)
ON_BN_CLICKED(IDC_SORTER_BAG5_OUT, OnSortEntryRemove5)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG5_OUT, OnSortEntryRemove5)
ON_BN_CLICKED(IDC_SORTER_BAG6_IN, OnSortEntryAdd6)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG6_IN, OnSortEntryAdd6)
ON_BN_CLICKED(IDC_SORTER_BAG6_OUT, OnSortEntryRemove6)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG6_OUT, OnSortEntryRemove6)
ON_BN_CLICKED(IDC_SORTER_BAG7_IN, OnSortEntryAdd7)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG7_IN, OnSortEntryAdd7)
ON_BN_CLICKED(IDC_SORTER_BAG7_OUT, OnSortEntryRemove7)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG7_OUT, OnSortEntryRemove7)
ON_BN_CLICKED(IDC_SORTER_BAG8_IN, OnSortEntryAdd8)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG8_IN, OnSortEntryAdd8)
ON_BN_CLICKED(IDC_SORTER_BAG8_OUT, OnSortEntryRemove8)
ON_BN_DOUBLECLICKED(IDC_SORTER_BAG8_OUT, OnSortEntryRemove8)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable() {
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

void CConfigDialog::disableControls() {
	for (int x = 0; x < 8; x++)
	{
		m_Bag[x].EnableWindow(false);
		m_BagIn[x].EnableWindow(false);
		m_BagOut[x].EnableWindow(false);
	}
}

void CConfigDialog::enableControls() {
	for (int x = 0; x < 8; x++)
	{
		m_Bag[x].EnableWindow(true);
		m_BagIn[x].EnableWindow(true);
		m_BagOut[x].EnableWindow(true);
	}
}

void CConfigDialog::configToControls(CConfigData *configData) {
	char buf[128];
	for (int x = 0; x < 8; x++)
	{
		while (m_Bag[x].GetCount())
			m_Bag[x].DeleteString(0);
	}
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			if (strlen(configData->sortBags[i].slotNr[j].itemName))
			{
				sprintf(buf, "%s", configData->sortBags[i].slotNr[j].itemName);
				m_Bag[i].AddString(buf);
			}
		}
	}
	reloadSortItems();
}

CConfigData * CConfigDialog::controlsToConfig() {
	CConfigData *newConfigData = new CConfigData();
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			newConfigData->sortBags[i].slotNr[0].itemName[0] = '\0';
		}
	}
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			m_Bag[i].GetText(j, newConfigData->sortBags[i].slotNr[j].itemName);
		}
	}

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) {
	if (nIDEvent == 1001)
	{
		KillTimer(1001);
		int firstOpenSlot = 0;
		for (int i = 0; i < 8; i++)
		{
			if (!m_Bag[i].GetCount())
			{
				firstOpenSlot = i;
				break;
			}
		}
		for (int i = 0; i < 8; i++)
		{
			if (firstOpenSlot != i && !m_Bag[i].GetCount())
			{
				m_Bag[i].EnableWindow(false);
				m_BagIn[i].ShowWindow(false);
				m_BagOut[i].ShowWindow(false);
			}
			else
			{
				m_Bag[i].EnableWindow(true);
				m_BagIn[i].ShowWindow(true);
				m_BagOut[i].ShowWindow(true);
			}
		}
		SetTimer(1001, 250, NULL);
	}
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(             m_enable);
	skin.SetButtonSkin(     m_OK);
	for(int loop = 0; loop < 8; loop++)
	{
		skin.SetButtonSkin(             m_BagIn[loop]);
		skin.SetButtonSkin(             m_BagOut[loop]);
	}
}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	SetTimer(1001, 250, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg) {
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable) {
	m_enable.SetCheck(enable);
}

void CConfigDialog::reloadSortItems() {
	CTibiaItemProxy itemProxy;
	while (m_sortItemList.GetCount() > 0)
		m_sortItemList.DeleteString(0);
	int count = itemProxy.getItemCount();
	for (int i = 0; i < count; i++)
		m_sortItemList.AddString(itemProxy.getItemNameAtIndex(i));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryAdd1() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];

	m_sortItemList.GetLBText(sel, itemName);

	if (sel == -1)
		return;

	m_Bag[0].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove1() {
	char itemName[128];
	int sel = m_Bag[0].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[0].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[0].DeleteString(sel);
}

void CConfigDialog::OnSortEntryAdd2() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];
	m_sortItemList.GetLBText(sel, itemName);
	if (sel == -1)
		return;
	m_Bag[1].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove2() {
	char itemName[128];
	int sel = m_Bag[1].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[1].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[1].DeleteString(sel);
}

void CConfigDialog::OnSortEntryAdd3() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];
	m_sortItemList.GetLBText(sel, itemName);
	if (sel == -1)
		return;
	m_Bag[2].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove3() {
	char itemName[128];
	int sel = m_Bag[2].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[2].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[2].DeleteString(sel);
}

void CConfigDialog::OnSortEntryAdd4() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];
	m_sortItemList.GetLBText(sel, itemName);
	if (sel == -1)
		return;
	m_Bag[3].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove4() {
	char itemName[128];
	int sel = m_Bag[3].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[3].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[3].DeleteString(sel);
}

void CConfigDialog::OnSortEntryAdd5() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];
	m_sortItemList.GetLBText(sel, itemName);
	if (sel == -1)
		return;
	m_Bag[4].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove5() {
	char itemName[128];
	int sel = m_Bag[4].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[4].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[4].DeleteString(sel);
}

void CConfigDialog::OnSortEntryAdd6() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];
	m_sortItemList.GetLBText(sel, itemName);
	if (sel == -1)
		return;
	m_Bag[5].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove6() {
	char itemName[128];
	int sel = m_Bag[5].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[5].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[5].DeleteString(sel);
}

void CConfigDialog::OnSortEntryAdd7() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];
	m_sortItemList.GetLBText(sel, itemName);
	if (sel == -1)
		return;
	m_Bag[6].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove7() {
	char itemName[128];
	int sel = m_Bag[6].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[6].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[6].DeleteString(sel);
}

void CConfigDialog::OnSortEntryAdd8() {
	int sel = m_sortItemList.GetCurSel();
	char itemName[64];
	m_sortItemList.GetLBText(sel, itemName);
	if (sel == -1)
		return;
	m_Bag[7].AddString(itemName);
	m_sortItemList.DeleteString(m_sortItemList.FindStringExact(-1, itemName));
	m_sortItemList.SetCurSel(0);
}

void CConfigDialog::OnSortEntryRemove8() {
	char itemName[128];
	int sel = m_Bag[7].GetCurSel();
	if (sel == -1)
		return;
	m_Bag[7].GetText(sel, itemName);
	m_sortItemList.AddString(itemName);
	m_sortItemList.SetCurSel(m_sortItemList.FindStringExact(-1, itemName));
	m_Bag[7].DeleteString(sel);
}
