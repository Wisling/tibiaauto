// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_aim.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_aimApp *app,CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_AUTOAIM_RUNELIST, m_RuneType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
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

	m_RuneType.EnableWindow(false);
}

void CConfigDialog::enableControls()
{

	m_RuneType.EnableWindow(true);
}



void CConfigDialog::configToControls(CConfigData *configData)
{

	for (int i=0;i<m_RuneType.GetCount();i++){
		if (int(m_RuneType.GetItemData(i)) == int(configData->RuneType)){
			m_RuneType.SetCurSel(i);
		}
	}
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CConfigData *newConfigData = new CConfigData();

	newConfigData->RuneType=m_RuneType.GetItemData(m_RuneType.GetCurSel());

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent) 
{

	
	CDialog::OnTimer(nIDEvent);
}

BOOL CConfigDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CTibiaItemProxy itemProxy;

	m_RuneType.InsertString(0,"(none)");
	m_RuneType.SetItemData(0,0);
	m_RuneType.InsertString(1,"LMM");
	m_RuneType.SetItemData(1,itemProxy.getValueForConst("runeLMM"));
	m_RuneType.InsertString(2,"HMM");
	m_RuneType.SetItemData(2,itemProxy.getValueForConst("runeHMM"));
	m_RuneType.InsertString(3,"GFB");
	m_RuneType.SetItemData(3,itemProxy.getValueForConst("runeGFB"));
	m_RuneType.InsertString(4,"EXPLO");
	m_RuneType.SetItemData(4,itemProxy.getValueForConst("runeExplo"));
	m_RuneType.InsertString(5,"SoulFire");
	m_RuneType.SetItemData(5,itemProxy.getValueForConst("runeSoulFire"));
	m_RuneType.InsertString(6,"SD");
	m_RuneType.SetItemData(6,itemProxy.getValueForConst("runeSD"));
	
	
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
