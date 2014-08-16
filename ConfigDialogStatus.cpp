// ConfigDialogStatus.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "ConfigDialogStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialogStatus dialog


CConfigDialogStatus::CConfigDialogStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialogStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialogStatus)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConfigDialogStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialogStatus)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDD_CONFIGSTATUS_LOG, m_log);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialogStatus, CDialog)
	//{{AFX_MSG_MAP(CConfigDialogStatus)
	ON_WM_CLOSE()
	ON_MESSAGE(MY_WM_MESSAGE_ADDTOLOG, OnMyMessageAddToLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialogStatus message handlers

void CConfigDialogStatus::resetLog()
{
	m_log.DeleteAllItems();
}

void CConfigDialogStatus::addToLog(char *message)
{
	m_log.InsertItem(0,message);
}

BOOL CConfigDialogStatus::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_log.InsertColumn(0,"Log message",LVCFMT_LEFT,299);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDialogStatus::disableOk()
{
	m_ok.EnableWindow(false);
}

void CConfigDialogStatus::enableOk()
{
	m_ok.EnableWindow(true);
}

void CConfigDialogStatus::OnClose()
{
		
}

void CConfigDialogStatus::OnOK()
{
	ShowWindow(SW_HIDE);
}


LRESULT CConfigDialogStatus::OnMyMessageAddToLog(UINT wParam, LONG lParam)
{
	addToLog((char *)wParam);
	return 0;
}

void CConfigDialogStatus::msgAddToLog(char *message)
{
	SendMessage(MY_WM_MESSAGE_ADDTOLOG,(unsigned int)message,NULL);
}
