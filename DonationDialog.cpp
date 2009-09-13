// DonationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "DonationDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDonationDialog dialog


CDonationDialog::CDonationDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(CDonationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDonationDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDonationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDonationDialog)
	DDX_Control(pDX, IDOK, m_OK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDonationDialog, CDialog)
	//{{AFX_MSG_MAP(CDonationDialog)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDonationDialog message handlers

BOOL CDonationDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(m_OK);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
