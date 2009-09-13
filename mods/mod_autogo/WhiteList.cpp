// WhiteList.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "WhiteList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWhiteList dialog


CWhiteList::CWhiteList(char whiteListParam[100][32],int * mkBlackParam, CWnd* pParent /*=NULL*/)
	: MyDialog(CWhiteList::IDD, pParent)
{
	whiteList = (char*)whiteListParam;
	mkBlack = mkBlackParam;
	//{{AFX_DATA_INIT(CWhiteList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWhiteList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhiteList)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_WHITE_LIST, m_WhiteListFrame);
	DDX_Control(pDX, IDC_WHITELIST_NAME, m_name);
	DDX_Control(pDX, IDC_WHITELIST_LIST, m_list);
	DDX_Control(pDX, IDC_WHITELIST_MKBLACK, m_mkblack);
	DDX_Control(pDX, IDC_WHITELIST_DELETE, m_delete);
	DDX_Control(pDX, IDC_WHITELIST_ADD, m_add);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWhiteList, CDialog)
	//{{AFX_MSG_MAP(CWhiteList)
	ON_BN_CLICKED(IDC_WHITELIST_ADD, OnWhitelistAdd)
	ON_BN_CLICKED(IDC_WHITELIST_DELETE, OnWhitelistDelete)
	ON_BN_CLICKED(IDC_WHITELIST_MKBLACK, OnWhitelistMkBlack)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhiteList message handlers

void CWhiteList::OnWhitelistMkBlack()
{
	if (m_mkblack.GetCheck())
		m_WhiteListFrame.SetWindowText("Black List");
	else 
		m_WhiteListFrame.SetWindowText("White List");
	Invalidate();
	*mkBlack=(m_mkblack.GetCheck()?1:0);
}

void CWhiteList::OnWhitelistAdd() 
{
	char lpBuffer[32];
	if (m_name.GetWindowText(lpBuffer,32)){
		m_list.AddString(lpBuffer);		
		m_name.SetWindowText("");
	}
	ListToMem();
}

void CWhiteList::OnWhitelistDelete() 
{
	int iCurSel = m_list.GetCurSel();
	if (iCurSel > -1){
		m_list.DeleteString(iCurSel);
	}
	ListToMem();
	
}

void CWhiteList::ListToMem()
{
	int iCount = m_list.GetCount();
	int i;
	char lpBuffer[32];
	for (i=0;i<iCount;i++){
		m_list.GetText(i,lpBuffer);
		memcpy(whiteList+i*32,lpBuffer,32);
	}
	memset(whiteList+(iCount)*32,0,32);
}

void CWhiteList::Mem2List()
{
	int i;
	char lpBuffer[32];
	for (i=0;i<100;i++){
		memcpy(lpBuffer,whiteList+i*32,32);
		lpBuffer[31]=0;
		if (IsCharAlphaNumeric(lpBuffer[0])){
			m_list.AddString(lpBuffer);
		}else{
			break;
		}
	}
}

BOOL CWhiteList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_delete);
	skin.SetButtonSkin(	m_add);
	skin.SetButtonSkin(	m_OK);


	m_mkblack.SetCheck(*mkBlack);
	if (m_mkblack.GetCheck())
		m_WhiteListFrame.SetWindowText("Black List");
	else 
		m_WhiteListFrame.SetWindowText("White List");
	Mem2List();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
