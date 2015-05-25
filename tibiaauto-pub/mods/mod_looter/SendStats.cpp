// SendStats.cpp : implementation file
//

#include "stdafx.h"
#include "mod_looter.h"
#include "SendStats.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSendStats dialog


CSendStats::CSendStats(CWnd* pParent /*=NULL*/)
	: CDialog(CSendStats::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendStats)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSendStats::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendStats)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendStats, CDialog)
//{{AFX_MSG_MAP(CSendStats)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendStats message handlers
