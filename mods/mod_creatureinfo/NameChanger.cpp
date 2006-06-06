// NameChanger.cpp : implementation file
//

#include "stdafx.h"
#include "mod_creatureinfo.h"
#include "NameChanger.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameChanger dialog


CNameChanger::CNameChanger(CWnd* pParent /*=NULL*/)
	: CDialog(CNameChanger::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameChanger)
	//}}AFX_DATA_INIT
}


void CNameChanger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameChanger)
	DDX_Control(pDX, IDC_TOOLCREATURINFO_BATTLEONLY, m_battleOnly);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_ADVANCED, m_advanced);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_RELPOS, m_relpos);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_REFRESH, m_refresh);
	DDX_Control(pDX, IDC_TOOLCREATURINFO_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNameChanger, CDialog)
	//{{AFX_MSG_MAP(CNameChanger)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOOLCREATURINFO_REFRESH, OnToolcreaturinfoRefresh)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_TOOLCREATURINFO_LIST, OnBeginlabeleditToolcreaturinfoList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TOOLCREATURINFO_LIST, OnEndlabeleditToolcreaturinfoList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_TOOLCREATURINFO_LIST, OnKeydownToolcreaturinfoList)
	ON_BN_CLICKED(IDC_TOOLCREATURINFO_ADVANCED, OnToolcreaturinfoAdvanced)
	ON_NOTIFY(NM_DBLCLK, IDC_TOOLCREATURINFO_LIST, OnDblclkToolcreaturinfoList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameChanger message handlers

void CNameChanger::RefreshInfo() 
{
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	char buffer[128];
	int chNr;
	int iCount = m_list.GetItemCount();
	int iAdded=0;

	CTibiaCharacter *self = reader.readSelfCharacter();

	m_list.SendMessage(WM_SETREDRAW,0,0);
	
	for (chNr=0;chNr<memConstData.m_memMaxCreatures;chNr++){
		CTibiaCharacter *ch = reader.readVisibleCreature(chNr);
		if (ch->name[0] && (m_battleOnly.GetCheck()?ch->visible&&ch->z==self->z:1)){
			//T4: We have a creature
			if (iAdded < iCount){
				m_list.SetItem(iAdded,0,LVIF_TEXT|LVIF_PARAM,ch->name,0,0,0,chNr);
			}else{
				m_list.InsertItem(iAdded,ch->name);
				m_list.SetItem(iAdded,0,LVIF_TEXT|LVIF_PARAM,ch->name,0,0,0,chNr);
			}

			sprintf(buffer,"%d%%",ch->hpPercLeft);
			m_list.SetItem(iAdded,1,LVIF_TEXT,buffer,0,0,0,0);

			if (m_relpos.GetCheck()){
				sprintf(buffer,"%d,%d,%d",ch->x-self->x,ch->y-self->y,ch->z-self->z);
			}else{
				sprintf(buffer,"%d,%d,%d",ch->x,ch->y,ch->z);
			}
			m_list.SetItem(iAdded,2,LVIF_TEXT,buffer,0,0,0,0);

			sprintf(buffer,"%d",chNr);
			m_list.SetItem(iAdded,3,LVIF_TEXT,buffer,0,0,0,0);

			if (m_advanced.GetCheck()){
				sprintf(buffer,"0x%08X",ch->tibiaId);
				m_list.SetItem(iAdded,4,LVIF_TEXT,buffer,0,0,0,0);
				sprintf(buffer,"%d",ch->outfitId);
				m_list.SetItem(iAdded,5,LVIF_TEXT,buffer,0,0,0,0);
				sprintf(buffer,"%d",ch->monsterType);
				m_list.SetItem(iAdded,6,LVIF_TEXT,buffer,0,0,0,0);
			}

			iAdded++;
		}

		delete ch;
	}
	//T4: Remove unused items
	for(;iCount>iAdded;iCount--){
		m_list.DeleteItem(iCount-1);
	}

	m_list.SendMessage(WM_SETREDRAW,1,0);
	m_list.RedrawWindow(NULL,NULL,FALSE);
	m_list.UpdateWindow();

	delete self;
}

BOOL CNameChanger::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitList();
	RefreshInfo();
	m_refresh.SetCheck(1);
	SetTimer(1000,1000,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNameChanger::InitList(){
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();

	m_list.InsertColumn(0,"Name",LVCFMT_LEFT,150);
	m_list.InsertColumn(1,"HP",LVCFMT_RIGHT,40);
	m_list.InsertColumn(2,"Position",LVCFMT_RIGHT,90);
	m_list.InsertColumn(3,"No",LVCFMT_RIGHT,30);

	int aCols[4]={3,0,1,2};
	m_list.SetColumnOrderArray(4,aCols);

	m_list.SetItemCount(memConstData.m_memMaxCreatures);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);	

}

void CNameChanger::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1000){
		RefreshInfo();
	}
	CDialog::OnTimer(nIDEvent);
}

void CNameChanger::OnToolcreaturinfoRefresh() 
{
	if (m_refresh.GetCheck()){
		SetTimer(1000,1000,NULL);
	}else{
		KillTimer(1000);
	}
}

void CNameChanger::OnBeginlabeleditToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	//T4: Limit name lenght to 31 chars
	CEdit *edit;
	edit = m_list.GetEditControl();
	edit->SetLimitText(31);

	//Suspend Timer
	KillTimer(1000);

	*pResult = 0;
}

void CNameChanger::OnEndlabeleditToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if (pDispInfo->item.pszText){
		CMemReaderProxy reader;
		LVITEM pItem;

		pItem.mask = LVIF_PARAM;
		pItem.iItem = pDispInfo->item.iItem;
		m_list.GetItem(&pItem);

		reader.writeVisibleCreatureName(pItem.lParam,pDispInfo->item.pszText);
		*pResult = 1;
	}else{
		*pResult = 0;
	}

	//Resume timer
	if (m_refresh.GetCheck()){
		SetTimer(1000,1000,NULL);
	}
}

void CNameChanger::OnKeydownToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	if (pLVKeyDow->wVKey == VK_F2){
		m_list.EditLabel(m_list.GetSelectionMark());
	}
	*pResult = 0;
}

void CNameChanger::OnToolcreaturinfoAdvanced() 
{
	if (m_advanced.GetCheck()){
		m_list.InsertColumn(4,"TibiaID",LVCFMT_RIGHT,80);
		m_list.InsertColumn(5,"OutfitID",LVCFMT_RIGHT,60);
		m_list.InsertColumn(6,"MonType",LVCFMT_RIGHT,60);
	}else{
		m_list.DeleteColumn(6);
		m_list.DeleteColumn(5);
		m_list.DeleteColumn(4);
	}
}

void CNameChanger::OnDblclkToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_list.EditLabel(m_list.GetSelectionMark());
	
	*pResult = 0;
}
