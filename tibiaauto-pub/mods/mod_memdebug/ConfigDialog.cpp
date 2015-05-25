// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_memdebug.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_memdebugApp *app, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	m_scan          = FALSE;
	m_error         = _T("");
	m_addressResDec = 0;
	m_address       = 0;
	m_addressResHex = _T("");
	m_val           = 0;
	//}}AFX_DATA_INIT
	m_app        = app;
	m_errorCount = 0;
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Check(pDX, IDC_SCAN, m_scan);
	DDX_Text(pDX, IDC_ERROR, m_error);
	DDX_Text(pDX, IDC_EDIT_ADDRESS_RES_DEC, m_addressResDec);
	DDX_Text(pDX, IDC_ADDRESS, m_address);
	DDX_Text(pDX, IDC_EDIT_ADDRESS_RES_HEX, m_addressResHex);
	DDX_Text(pDX, IDC_VAL, m_val);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_CLOSE()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_TEST, OnTest)
ON_BN_CLICKED(IDC_TEST2, OnTest2)
ON_BN_CLICKED(IDC_SCAN, OnScan)

//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnTest()
{
	UpdateData(FALSE);

	CMemReaderProxy reader;

	reader.setMemIntValue(0x00400000 + m_address, m_val);
}

void CConfigDialog::OnTest2()
{
}

void CConfigDialog::OnScan()
{
	UpdateData(true);

	if (m_scan)
	{
		m_valueScanIter = 0;

		RegisterDebug("memory scanning started");
		SetTimer(1000, 100, NULL);
	}
	else
	{
		RegisterDebug("memory scanning stopped");
		this->KillTimer(1000);
	}
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1000)
	{
		CMemReaderProxy reader;

		char buf[128];

		UpdateData(true);

		int value = reader.getMemIntValue(0x00400000 + m_address);


		m_addressResDec = value;

		sprintf(buf, "0x%x", value);
		m_addressResHex = buf;

		UpdateData(false);
	}
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::RegisterError(char *msg)
{
	char buf[1024];
	sprintf(buf, "[%d] ERROR: %s", ++m_errorCount, msg);
	m_error = buf;
	UpdateData(false);
}

void CConfigDialog::RegisterDebug(char *msg)
{
	char buf[1024];
	sprintf(buf, "[%d] INFO: %s", ++m_errorCount, msg);
	m_error = buf;
	UpdateData(false);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}
