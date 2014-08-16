// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_xray.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include "IPCBackPipeProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_xrayApp *app,CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_EXTRAHOTKEYS, m_extrahotkeys);
	DDX_Control(pDX, IDC_AUTORESET, m_autoreset);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
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

void CConfigDialog::OnEnable()
{
	if (m_enable.GetCheck())
	{
		m_app->controlsToConfig();
		if (m_app->validateConfig(1))
		{
			m_app->start();
		} else {
			m_enable.SetCheck(0);
		}
	} else {
		m_app->stop();
	}
}

void CConfigDialog::disableControls()
{
	m_autoreset.EnableWindow(false);
	m_extrahotkeys.EnableWindow(false);

}

void CConfigDialog::enableControls()
{
	m_autoreset.EnableWindow(true);
	m_extrahotkeys.EnableWindow(true);

}



void CConfigDialog::configToControls(CConfigData *configData)
{
	//char buf[128];
	m_autoreset.SetCheck(configData->autoreset);
	m_extrahotkeys.SetCheck(configData->extrahotkeys);
	
}

CConfigData * CConfigDialog::controlsToConfig()
{
	//char buf[128];
	CConfigData *newConfigData = new CConfigData();

	newConfigData->autoreset=m_autoreset.GetCheck();
	newConfigData->extrahotkeys=m_extrahotkeys.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{

	
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_enable);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}

void CConfigDialog::refreshFishStatus()
{
	
}
