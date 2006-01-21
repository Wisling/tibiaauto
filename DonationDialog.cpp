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
	: CDialog(CDonationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDonationDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDonationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDonationDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDonationDialog, CDialog)
	//{{AFX_MSG_MAP(CDonationDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDonationDialog message handlers
