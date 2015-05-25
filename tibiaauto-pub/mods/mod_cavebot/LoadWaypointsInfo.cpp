// LoadWaypointsInfo.cpp : implementation file
//

#include "stdafx.h"
#include "mod_cavebot.h"
#include "LoadWaypointsInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLoadWaypointsInfo dialog


CLoadWaypointsInfo::CLoadWaypointsInfo(char* a_prefix, CWnd* pParent /*=NULL*/)
	: MyDialog(CLoadWaypointsInfo::IDD, pParent)
{
	prefix = a_prefix;
	//{{AFX_DATA_INIT(CLoadWaypointsInfo)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLoadWaypointsInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadWaypointsInfo)
	DDX_Control(pDX, IDC_PREFIX, m_prefix);
	DDX_Control(pDX, IDC_FRAME, m_frame);
	DDX_Control(pDX, IDYES, m_YES);
	DDX_Control(pDX, IDCANCEL, m_CANCEL);
	DDX_Control(pDX, IDNO, m_NO);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLoadWaypointsInfo, CDialog)
//{{AFX_MSG_MAP(CLoadWaypointsInfo)
ON_BN_CLICKED(IDYES, OnYES)
ON_BN_CLICKED(IDNO, OnNO)
ON_WM_ERASEBKGND()
ON_WM_DRAWITEM()
ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadWaypointsInfo message handlers

void CLoadWaypointsInfo::OnYES()
{
	m_prefix.GetWindowText(prefix, 511);
	this->EndDialog(IDYES);
}

void CLoadWaypointsInfo::OnNO()
{
	m_prefix.GetWindowText(prefix, 511);
	this->EndDialog(IDNO);
}

BOOL CLoadWaypointsInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(m_YES);
	skin.SetButtonSkin(m_NO);
	skin.SetButtonSkin(m_CANCEL);
	m_prefix.SetWindowText("");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
