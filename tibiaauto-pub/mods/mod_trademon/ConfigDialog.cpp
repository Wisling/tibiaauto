// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_trademon.h"
#include "ConfigDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_trademonApp *app, CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app = app;
}


void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_FRAME_TRADE_TALKER, m_TradeTalkerFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_TRADETOOL_CHANNEL, m_channel);
	DDX_Control(pDX, IDC_TRADETOOL_CHANNELINTERVAL, m_channelInterval);
	DDX_Control(pDX, IDC_TRADETOOL_TRADETALK, m_tradeTalker);
	DDX_Control(pDX, IDC_TRADETOOL_YELLINTERVAL, m_yellInterval);
	DDX_Control(pDX, IDC_TRADETOOL_YELL, m_yell);
	DDX_Control(pDX, IDC_TRADETOOL_TRADEMON, m_tradeMon);
	DDX_Control(pDX, IDC_TRADETOOL_SAYINTERVAL, m_sayInterval);
	DDX_Control(pDX, IDC_TRADETOOL_SAY, m_say);
	DDX_Control(pDX, IDC_TRADETOOL_MESSAGE, m_message);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_BN_CLICKED(IDC_TRADETOOL_CHANNEL, OnTradetoolChannel)
ON_BN_CLICKED(IDC_TRADETOOL_SAY, OnTradetoolSay)
ON_BN_CLICKED(IDC_TRADETOOL_YELL, OnTradetoolYell)
ON_BN_CLICKED(IDC_TRADETOOL_TRADETALK, OnTradetoolTradetalk)
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
			m_app->start();
		else
			m_enable.SetCheck(0);
	}
	else
	{
		m_app->stop();
	}
}

void CConfigDialog::disableControls()
{
	m_tradeTalker.EnableWindow(false);
	m_message.EnableWindow(false);
	m_channel.EnableWindow(false);
	m_channelInterval.EnableWindow(false);
	m_yell.EnableWindow(false);
	m_yellInterval.EnableWindow(false);
	m_say.EnableWindow(false);
	m_sayInterval.EnableWindow(false);

	m_tradeMon.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_tradeTalker.EnableWindow(true);
	if (m_tradeTalker.GetCheck())
	{
		m_message.EnableWindow(true);
		m_channel.EnableWindow(true);
		m_channelInterval.EnableWindow(true);
		m_yell.EnableWindow(true);
		m_yellInterval.EnableWindow(true);
		m_say.EnableWindow(true);
		m_sayInterval.EnableWindow(true);
	}
	m_tradeMon.EnableWindow(true);
}


void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	m_tradeTalker.SetCheck(configData->tradeTalk);
	m_message.SetWindowText(configData->message);
	m_channel.SetCheck(configData->channel);
	sprintf(buf, "%d", configData->channelInterval);  m_channelInterval.SetWindowText(buf);
	m_yell.SetCheck(configData->yell);
	sprintf(buf, "%d", configData->yellInterval);     m_yellInterval.SetWindowText(buf);
	m_say.SetCheck(configData->say);
	sprintf(buf, "%d", configData->sayInterval);      m_sayInterval.SetWindowText(buf);
	m_tradeMon.SetCheck(configData->tradeMon);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	newConfigData->tradeTalk = m_tradeTalker.GetCheck();
	m_message.GetWindowText(newConfigData->message, 255);
	newConfigData->message[255]                                               = 0;
	newConfigData->channel                                                    = m_channel.GetCheck();
	m_channelInterval.GetWindowText(buf, 128); newConfigData->channelInterval = atoi(buf);
	newConfigData->yell                                                       = m_yell.GetCheck();
	m_yellInterval.GetWindowText(buf, 128); newConfigData->yellInterval       = atoi(buf);
	newConfigData->say                                                        = m_say.GetCheck();
	m_sayInterval.GetWindowText(buf, 128); newConfigData->sayInterval         = atoi(buf);
	newConfigData->tradeMon                                                   = m_tradeMon.GetCheck();

	return newConfigData;
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(     m_OK);
	skin.SetButtonSkin(     m_enable);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	m_message.EnableWindow(false);
	m_channel.EnableWindow(false);
	m_channelInterval.EnableWindow(false);
	m_yell.EnableWindow(false);
	m_yellInterval.EnableWindow(false);
	m_say.EnableWindow(false);
	m_sayInterval.EnableWindow(false);


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

void CConfigDialog::OnTradetoolChannel()
{
	// TODO: Add your control notification handler code here
}

void CConfigDialog::OnTradetoolSay()
{
	// TODO: Add your control notification handler code here
}

void CConfigDialog::OnTradetoolYell()
{
	// TODO: Add your control notification handler code here
}

void CConfigDialog::OnTradetoolTradetalk()
{
	if (m_tradeTalker.GetCheck())
	{
		m_message.EnableWindow(true);
		m_channel.EnableWindow(true);
		m_channelInterval.EnableWindow(true);
		m_yell.EnableWindow(true);
		m_yellInterval.EnableWindow(true);
		m_say.EnableWindow(true);
		m_sayInterval.EnableWindow(true);
	}
	else
	{
		m_message.EnableWindow(false);
		m_channel.EnableWindow(false);
		m_channelInterval.EnableWindow(false);
		m_yell.EnableWindow(false);
		m_yellInterval.EnableWindow(false);
		m_say.EnableWindow(false);
		m_sayInterval.EnableWindow(false);
	}
}
