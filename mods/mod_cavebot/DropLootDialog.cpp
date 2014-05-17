// DropLootDialog .cpp : implementation file
//

#include "stdafx.h"
#include "mod_cavebot.h"
#include "DropLootDialog.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DropLootDialog  dialog
const MAX_DROP_ITEMS = 100;

CDropLootDialog::CDropLootDialog(char dropListParam[MAX_DROP_ITEMS][64],int& dropListCountParam,int& dropWhenCapacityLimitReachedParam,int& dropOnlyLootedParam, CWnd* pParent /*= NULL*/)
	: MyDialog(CDropLootDialog::IDD, pParent)
{
	dropList=(char*)dropListParam;
	dropListCount=&dropListCountParam;
	dropWhenCapacityLimitReached=&dropWhenCapacityLimitReachedParam;
	dropOnlyLooted=&dropOnlyLootedParam;

	//{{AFX_DATA_INIT(CDropLootDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDropLootDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDropLootDialog)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_DROPLOOT, m_DropLootFrame);
	DDX_Control(pDX, IDC_DROPLIST_ITEMLIST, m_itemList);
	DDX_Control(pDX, IDC_DROPLIST_DROPLIST, m_dropList);
	DDX_Control(pDX, IDC_DROPLIST_WHENNOCAPS, m_dropWhenCapacityLimitReached);
	DDX_Control(pDX, IDC_DROPLIST_ONLYLOOT, m_dropOnlyLooted);
	DDX_Control(pDX, IDC_DROPLIST_ADD, m_add);
	DDX_Control(pDX, IDC_DROPLIST_DELETE, m_delete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDropLootDialog, CDialog)
	//{{AFX_MSG_MAP(CDropLootDialog)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDC_DROPLIST_ADD, OnDroplistAdd)
	ON_BN_DOUBLECLICKED(IDC_DROPLIST_ADD, OnDroplistAdd)
	ON_BN_CLICKED(IDC_DROPLIST_DELETE, OnDroplistDelete)
	ON_BN_DOUBLECLICKED(IDC_DROPLIST_DELETE, OnDroplistDelete)
	ON_BN_CLICKED(IDC_DROPLIST_WHENNOCAPS, OnDroplistDropWhenCapacityLimitReached)
	ON_BN_CLICKED(IDC_DROPLIST_ONLYLOOT, OnDroplistDropOnlyLooted)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DropLootDialog  message handlers

void CDropLootDialog::OnOK(){
	*dropListCount=0;
	for (int i=0; i<m_dropList.GetCount();i++){
		int len=m_dropList.GetTextLen(i);
		if (len < 64){
			m_dropList.GetText(i,dropList+*dropListCount*64);
			*dropListCount+=1;
		}
	}
	*dropWhenCapacityLimitReached=m_dropWhenCapacityLimitReached.GetCheck();
	*dropOnlyLooted=m_dropOnlyLooted.GetCheck();

	CDialog::OnOK();
}
void CDropLootDialog::OnDroplistAdd(){
	if (m_dropList.GetCount()>=MAX_DROP_ITEMS){
		AfxMessageBox("Limit Reached");
		return;
	}
	int ind=m_itemList.GetCurSel();
	if (ind >= 0 && ind < m_itemList.GetCount()){
		char itemName[64];
		int len=m_itemList.GetLBTextLen(ind);
		if (len < 64){
			m_itemList.GetLBText(ind,itemName);
			m_dropList.AddString(itemName);
		}
		m_itemList.DeleteString(ind);
		m_itemList.SetCurSel(min(m_itemList.GetCount()-1,ind));
	}
	
}
void CDropLootDialog::OnDroplistDelete(){
	int ind=m_dropList.GetCurSel();
	if (ind >= 0 && ind < m_dropList.GetCount()){
		char itemName[64];
		int len=m_dropList.GetTextLen(ind);
		if (len < 64){
			m_dropList.GetText(ind,itemName);
			m_itemList.AddString(itemName);
		}
		m_dropList.DeleteString(ind);
		m_dropList.SetCurSel(min(m_dropList.GetCount()-1,ind));
	}
}
void CDropLootDialog::OnDroplistDropWhenCapacityLimitReached(){
	int check1 = m_dropWhenCapacityLimitReached.GetCheck();
	int check2 = m_dropOnlyLooted.GetCheck();
	m_dropOnlyLooted.EnableWindow(check1);
	int enableList=!(check1 && check2);
	m_itemList.EnableWindow(enableList);
	m_dropList.EnableWindow(enableList);
	m_add.EnableWindow(enableList);
	m_delete.EnableWindow(enableList);
}
void CDropLootDialog::OnDroplistDropOnlyLooted(){
	int check1 = m_dropWhenCapacityLimitReached.GetCheck();
	int check2 = m_dropOnlyLooted.GetCheck();
	int enableList=!(check1 && check2);
	m_itemList.EnableWindow(enableList);
	m_dropList.EnableWindow(enableList);
	m_add.EnableWindow(enableList);
	m_delete.EnableWindow(enableList);
}
BOOL CDropLootDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(m_add);
	skin.SetButtonSkin(m_delete);
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_cancel);
	LoadItemList();
	m_dropWhenCapacityLimitReached.SetCheck(*dropWhenCapacityLimitReached);
	m_dropOnlyLooted.SetCheck(*dropOnlyLooted);
	OnDroplistDropWhenCapacityLimitReached();
	OnDroplistDropOnlyLooted();

	return TRUE;
}
void CDropLootDialog::LoadItemList(){
	CTibiaItemProxy itemProxy;

	// load items for item list combo box
	int count = itemProxy.getItemCount();
	int i;
	for (i=0;i<count;i++)
	{
		m_itemList.AddString(itemProxy.getItemNameAtIndex(i));
	}
	m_itemList.SetCurSel(0);

	for (i=0;i<*dropListCount && i<MAX_DROP_ITEMS;i++)
	{
		m_dropList.AddString(dropList+i*64);
	}
	int j;
	int found = 0;
	for (i=j=0;i<m_itemList.GetCount() && j<m_dropList.GetCount();)
	{
		char txt1[64];
		char txt2[64];
		int len1 = m_itemList.GetLBTextLen(i);
		int len2 = m_dropList.GetTextLen(j);
		if (len1<64 && len2<64){
			m_itemList.GetLBText(i,txt1);
			m_dropList.GetText(j,txt2);
			int comp=strcmp(txt1,txt2);
			switch (comp){
			case 0:
				m_itemList.DeleteString(i);
				found = 1;
				break;
			case 1:
				{
					if (!found){
						m_dropList.DeleteString(j);
					} else {
						j++;
						found = 0;
					}
				}
				break;
			case -1:
				i++;
				break;
			}
		}
		i+=(len1>=64);
		j+=(len2>=64);
	}
	if (m_itemList.GetCount()) m_itemList.SetCurSel(0);
	if (m_dropList.GetCount()) m_dropList.SetCurSel(0);
}
