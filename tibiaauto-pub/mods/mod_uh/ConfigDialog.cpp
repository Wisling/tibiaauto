// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_uh.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_uhApp *app,CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_FRAME_SELF_HEALING, m_SelfHealingFrame);
	DDX_Control(pDX, IDC_FRAME_GROUP_HEALING, m_GroupHealingFrame);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_GRP_MEMBER_REMOVE, m_memberRemove);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_GRP_MEMBER_ADD, m_memberAdd);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_GRP_MEMBER_LIST, m_memberList);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_GRP_MEMBER, m_member);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_GRP_RUNETYPE, m_grpRunetype);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_UHBORDERLINE, m_selfBorderline);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_SLEEP, m_sleepAfter);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_RUNETYPE, m_selfRunetype);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_HOTKEY, m_selfHotkey);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_GRP_HPBELOW, m_grpBorderline);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_GRP_FALLBACK, m_grpFallback);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_FALLBACK, m_selfFallback);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOOL_AUTOUH_GRP_MEMBER_ADD, OnToolAutouhGrpMemberAdd)
	ON_BN_CLICKED(IDC_TOOL_AUTOUH_GRP_MEMBER_REMOVE, OnToolAutouhGrpMemberRemove)
	ON_BN_DOUBLECLICKED(IDC_TOOL_AUTOUH_GRP_MEMBER_REMOVE, OnToolAutouhGrpMemberRemove)
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
	m_grpRunetype.EnableWindow(false);
	m_selfBorderline.EnableWindow(false);
	m_sleepAfter.EnableWindow(false);
	m_selfRunetype.EnableWindow(false);
	m_selfHotkey.EnableWindow(false);
	m_grpBorderline.EnableWindow(false);
	m_grpFallback.EnableWindow(false);
	m_selfFallback.EnableWindow(false);
	m_member.EnableWindow(false);
	m_memberList.EnableWindow(false);
	m_memberAdd.EnableWindow(false);
	m_memberRemove.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_grpRunetype.EnableWindow(true);
	m_selfBorderline.EnableWindow(true);
	m_sleepAfter.EnableWindow(true);
	m_selfRunetype.EnableWindow(true);
	m_selfHotkey.EnableWindow(true);
	m_grpBorderline.EnableWindow(true);
	m_grpFallback.EnableWindow(true);
	m_selfFallback.EnableWindow(true);
	m_member.EnableWindow(true);
	m_memberList.EnableWindow(true);
	m_memberAdd.EnableWindow(true);
	m_memberRemove.EnableWindow(true);
}



void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	sprintf(buf,"%d",configData->m_grpBorderline);m_grpBorderline.SetWindowText(buf);
	m_grpFallback.SetCheck(configData->m_grpFallback);
	m_grpRunetype.SetCurSel(configData->m_grpRunetype);
	
	sprintf(buf,"%d",configData->m_uhBorderline);m_selfBorderline.SetWindowText(buf);
	m_selfFallback.SetCheck(configData->m_fallback);
	m_selfRunetype.SetCurSel(configData->m_runetype);
	m_selfHotkey.SetCheck(configData->m_hotkeySelf);
	
	sprintf(buf,"%d",configData->m_sleepAfter);m_sleepAfter.SetWindowText(buf);

	int i;
	while (m_memberList.GetCount())
		m_memberList.DeleteString(0);
	for (i=0;i<configData->m_grpMemberCount;i++)
	{
		m_memberList.AddString(configData->m_grpMemberList[i]);
	}
}

CConfigData * CConfigDialog::controlsToConfig()
{
	int i;
	CConfigData *newConfigData = new CConfigData();

	char buf[128];

	m_grpBorderline.GetWindowText(buf,120);
	newConfigData->m_grpBorderline=atoi(buf);
	newConfigData->m_grpFallback=m_grpFallback.GetCheck();
	newConfigData->m_grpRunetype=m_grpRunetype.GetCurSel();

	m_selfBorderline.GetWindowText(buf,120);
	newConfigData->m_uhBorderline=atoi(buf);
	newConfigData->m_fallback=m_selfFallback.GetCheck();
	newConfigData->m_runetype=m_selfRunetype.GetCurSel();
	newConfigData->m_hotkeySelf=m_selfHotkey.GetCheck();
	
	m_sleepAfter.GetWindowText(buf,120);
	newConfigData->m_sleepAfter=atoi(buf);

	sprintf(buf,"%d",m_memberList.GetCount());
	newConfigData->m_grpMemberCount=m_memberList.GetCount();
	for (i=0;i<newConfigData->m_grpMemberCount;i++)
	{
		m_memberList.GetText(i,newConfigData->m_grpMemberList[i]);
	}
	

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_memberRemove);
	skin.SetButtonSkin(	m_memberAdd);
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
 

void CConfigDialog::OnToolAutouhGrpMemberAdd()
{
	char buf[64];
	m_member.GetWindowText(buf,63);
	m_memberList.AddString(buf);
	m_member.SetWindowText("");
}

void CConfigDialog::OnToolAutouhGrpMemberRemove()
{
	int curSel=m_memberList.GetCurSel();
	if (curSel!=LB_ERR)
	{
		char buf[64];
		m_memberList.GetText(curSel,buf);
		m_memberList.DeleteString(curSel);
		m_member.SetWindowText(buf);
	}
}
