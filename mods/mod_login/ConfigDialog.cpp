// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_login.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_loginApp *app,CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app=app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DROPEMPTY, m_dropEmpty);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HOTKEY_MANA, m_hotkeyMana);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HOTKEY_LIFE, m_hotkeyLife);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_SLEEP, m_sleep);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_MANABELOW, m_manaBelow);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HPBELOW, m_hpBelow);	
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKMANA, m_drinkMana);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKHP, m_drinkHp);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
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
	m_dropEmpty.EnableWindow(false);
	m_hotkeyMana.EnableWindow(false);
	m_hotkeyLife.EnableWindow(false);
	m_sleep.EnableWindow(false);
	m_manaBelow.EnableWindow(false);
	m_hpBelow.EnableWindow(false);	
	m_drinkMana.EnableWindow(false);
	m_drinkHp.EnableWindow(false);

}

void CConfigDialog::enableControls()
{
	m_dropEmpty.EnableWindow(true);
	m_hotkeyMana.EnableWindow(true);
	m_hotkeyLife.EnableWindow(true);
	m_sleep.EnableWindow(true);
	m_manaBelow.EnableWindow(true);
	m_hpBelow.EnableWindow(true);	
	m_drinkMana.EnableWindow(true);
	m_drinkHp.EnableWindow(true);
}



void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	m_dropEmpty.SetCheck(configData->dropEmpty);
	m_hotkeyMana.SetCheck(configData->hotkeyMana);
	m_hotkeyLife.SetCheck(configData->hotkeyHp);
	sprintf(buf,"%d",configData->sleep);m_sleep.SetWindowText(buf);
	sprintf(buf,"%d",configData->manaBelow);m_manaBelow.SetWindowText(buf);
	sprintf(buf,"%d",configData->hpBelow);m_hpBelow.SetWindowText(buf);
	m_drinkMana.SetCheck(configData->drinkMana);
	m_drinkHp.SetCheck(configData->drinkHp);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	newConfigData->dropEmpty=m_dropEmpty.GetCheck();
	newConfigData->hotkeyMana=m_hotkeyMana.GetCheck();
	newConfigData->hotkeyHp=m_hotkeyLife.GetCheck();
	m_sleep.GetWindowText(buf,127);newConfigData->sleep=atoi(buf);
	m_manaBelow.GetWindowText(buf,127);newConfigData->manaBelow=atoi(buf);
	m_hpBelow.GetWindowText(buf,127);newConfigData->hpBelow=atoi(buf);
	newConfigData->drinkMana=m_drinkMana.GetCheck();
	newConfigData->drinkHp=m_drinkHp.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) 
{

	
	CDialog::OnTimer(nIDEvent);
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

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}
