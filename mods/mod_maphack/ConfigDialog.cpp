// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_maphack.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_maphackApp *app,CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_FRAME_MAP_OPTIONS, m_MapOptionFrame);
	DDX_Control(pDX, IDC_REVEALCNAME, m_revealCName);
	DDX_Control(pDX, IDC_REVEAL_NOFISH, m_revealNoFish);
	DDX_Control(pDX, IDC_REVEALINVISIBLE, m_revealInvisible);
	DDX_Control(pDX, IDC_MINIMAPRESEARCH, m_minimapResearch);
	DDX_Control(pDX, IDC_SMOOTHWALKING, m_smoothWalking);
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
	m_revealNoFish.EnableWindow(false);
	m_revealCName.EnableWindow(false);	
	m_revealInvisible.EnableWindow(false);	
	m_minimapResearch.EnableWindow(false);
}	

void CConfigDialog::enableControls()
{
	m_revealNoFish.EnableWindow(true);
	m_revealCName.EnableWindow(true);
	m_revealInvisible.EnableWindow(true);
	m_minimapResearch.EnableWindow(true);
}



void CConfigDialog::configToControls(CConfigData *configData)
{
	m_revealNoFish.SetCheck(configData->revealNoFish);
	m_revealCName.SetCheck(configData->revealCName);
	m_revealInvisible.SetCheck(configData->revealInvisible);
	m_minimapResearch.SetCheck(configData->minimapResearch);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CConfigData *newConfigData = new CConfigData();

	newConfigData->revealNoFish=m_revealNoFish.GetCheck();
	newConfigData->revealCName=m_revealCName.GetCheck();	
	newConfigData->revealInvisible=m_revealInvisible.GetCheck();
	newConfigData->minimapResearch=m_minimapResearch.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) 
{

	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_enable);
	
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
