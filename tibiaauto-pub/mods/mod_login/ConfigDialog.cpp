// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_login.h"
#include "ConfigDialog.h"
#include <MemReader.h>
#include <queue>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

extern time_t loginTime;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

char* queue2Message = "Starting Log";

CConfigDialog::CConfigDialog(CMod_loginApp *app, CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_FRAME_LOGIN_SETTINGS, m_LoginSettingsFrame);
	DDX_Control(pDX, IDC_FRAME_DEBUG_LOG, m_DebugLogFrame);
	DDX_Control(pDX, IDC_FRAME_BACKPACK_CONTROL, m_BackpackControlFrame);
	DDX_Control(pDX, IDC_OPEN_CONTAINER8, m_openCont8);
	DDX_Control(pDX, IDC_OPEN_CONTAINER7, m_openCont7);
	DDX_Control(pDX, IDC_OPEN_CONTAINER6, m_openCont6);
	DDX_Control(pDX, IDC_OPEN_CONTAINER5, m_openCont5);
	DDX_Control(pDX, IDC_OPEN_CONTAINER4, m_openCont4);
	DDX_Control(pDX, IDC_OPEN_CONTAINER3, m_openCont3);
	DDX_Control(pDX, IDC_OPEN_CONTAINER2, m_openCont2);
	DDX_Control(pDX, IDC_OPEN_CONTAINER1, m_openCont1);
	DDX_Control(pDX, IDC_OPEN_BACKPACK, m_openMain);
	DDX_Control(pDX, IDC_CHAR_POS, m_charPos);
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_PASSWORD, m_password);
	DDX_Control(pDX, IDC_DEBUG, m_debug);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER, m_accountNumber);
	DDX_Control(pDX, IDC_LOGIN_DELAY, m_loginDelay);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_AUTOPASS, m_autopass);
	DDX_Control(pDX, IDC_LOGIN_AFTER_KILLED, m_loginAfterKilled);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
//{{AFX_MSG_MAP(CConfigDialog)
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_ENABLE, OnEnable)
ON_WM_TIMER()
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_AUTOPASS, OnAutopass)
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
	m_accountNumber.EnableWindow(false);
	m_password.EnableWindow(false);
	m_charPos.EnableWindow(false);
	m_openMain.EnableWindow(false);
	m_openCont1.EnableWindow(false);
	m_openCont2.EnableWindow(false);
	m_openCont3.EnableWindow(false);
	m_openCont4.EnableWindow(false);
	m_openCont5.EnableWindow(false);
	m_openCont6.EnableWindow(false);
	m_openCont7.EnableWindow(false);
	m_openCont8.EnableWindow(false);
	m_loginDelay.EnableWindow(false);
	m_autopass.EnableWindow(false);
	m_loginAfterKilled.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_charPos.EnableWindow(true);
	m_openMain.EnableWindow(true);
	m_openCont1.EnableWindow(true);
	m_openCont2.EnableWindow(true);
	m_openCont3.EnableWindow(true);
	m_openCont4.EnableWindow(true);
	m_openCont5.EnableWindow(true);
	m_openCont6.EnableWindow(true);
	m_openCont7.EnableWindow(true);
	m_openCont8.EnableWindow(true);
	m_loginDelay.EnableWindow(true);
	m_loginAfterKilled.EnableWindow(true);

	m_autopass.EnableWindow(false);
	OnAutopass();
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];

	sprintf(buf, "%s", configData->accountNumber);
	m_accountNumber.SetWindowText(buf);
	sprintf(buf, "%s", configData->password);
	m_password.SetWindowText(buf);
	sprintf(buf, "%d", configData->charPos);
	m_charPos.SetWindowText(buf);
	m_openMain.SetCheck(configData->openMain);
	m_openCont1.SetCheck(configData->openCont1);
	m_openCont2.SetCheck(configData->openCont2);
	m_openCont3.SetCheck(configData->openCont3);
	m_openCont4.SetCheck(configData->openCont4);
	m_openCont5.SetCheck(configData->openCont5);
	m_openCont6.SetCheck(configData->openCont6);
	m_openCont7.SetCheck(configData->openCont7);
	m_openCont8.SetCheck(configData->openCont8);
	sprintf(buf, "%d", configData->loginDelay);
	m_loginDelay.SetWindowText(buf);
	m_loginAfterKilled.SetCheck(configData->loginAfterKilled);

	m_autopass.SetCheck(0);// configData->autopass);
	m_autopass.EnableWindow(false);
	OnAutopass();
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	m_accountNumber.GetWindowText(newConfigData->accountNumber, 63);
	m_password.GetWindowText(newConfigData->password, 63);
	m_charPos.GetWindowText(buf, 63);
	newConfigData->charPos   = atoi(buf);
	newConfigData->openMain  = m_openMain.GetCheck();
	newConfigData->openCont1 = m_openCont1.GetCheck();
	newConfigData->openCont2 = m_openCont2.GetCheck();
	newConfigData->openCont3 = m_openCont3.GetCheck();
	newConfigData->openCont4 = m_openCont4.GetCheck();
	newConfigData->openCont5 = m_openCont5.GetCheck();
	newConfigData->openCont6 = m_openCont6.GetCheck();
	newConfigData->openCont7 = m_openCont7.GetCheck();
	newConfigData->openCont8 = m_openCont8.GetCheck();
	m_loginDelay.GetWindowText(buf, 63);
	newConfigData->loginDelay       = atoi(buf);
	newConfigData->autopass         = 0;                               // m_autopass.GetCheck();
	newConfigData->loginAfterKilled = m_loginAfterKilled.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1001)
	{
		KillTimer(1001);
		CMemReader& reader = CMemReader::getMemReader();
		switch (reader.getConnectionState())
		{
		case 0:
			if (loginTime)
			{
				char buf[128];
				if (loginTime - time(NULL) >= 0)
					sprintf(buf, "Connection status: waiting to log in %d seconds.", loginTime - time(NULL));
				else
					sprintf(buf, "Connection status: trying to log in for %d seconds.", time(NULL) - loginTime);
				m_status.SetWindowText(buf);
				break;
			}
			m_status.SetWindowText("Connection status: not connected");
			break;
		case 1:
			m_status.SetWindowText("Connection status: opening to login server");
			break;
		case 2:
			m_status.SetWindowText("Connection status: connecting to login server");
			break;
		case 3:
			m_status.SetWindowText("Connection status: disconnecting from login server");
			break;
		case 4:
			m_status.SetWindowText("Connection status: connected to login server");
			break;
		case 5:
			m_status.SetWindowText("Connection status: opening");
			break;
		case 6:
			m_status.SetWindowText("Connection status: connecting");
			break;
		case 7:
			m_status.SetWindowText("Connection status: disconnecting");
			break;
		case 9:
			m_status.SetWindowText("Connection status: pending");
			break;
		case 10:
			m_status.SetWindowText("Connection status: state 10");
			break;
		case 11:
			m_status.SetWindowText("Connection status: connected");
			break;
		default:
			m_status.SetWindowText("Connection status: unknown");
			break;
		};


		if (queue2Message)
		{
			char timeBuf[256];
			m_debug.InsertItem(0, "");
			time_t nowSec  = time(NULL);
			struct tm *now = localtime(&nowSec);
			sprintf(timeBuf, "%d:%02d:%02d", now->tm_hour, now->tm_min, now->tm_sec);
			m_debug.SetItemText(0, 0, timeBuf);
			m_debug.SetItemText(0, 1, queue2Message);
			if (m_debug.GetItemCount() > 100)
				m_debug.DeleteItem(m_debug.GetItemCount());
			queue2Message = NULL;
		}
		SetTimer(1001, 500, NULL);
	}

	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_enable);
	skin.SetButtonSkin(m_OK);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	m_debug.InsertColumn(0, "time", LVCFMT_LEFT, 65);
	m_debug.InsertColumn(1, "message", LVCFMT_LEFT, 240);

	SetTimer(1001, 500, NULL);


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

void CConfigDialog::OnAutopass()
{
	int enable = m_autopass.GetCheck();
	m_accountNumber.EnableWindow(!enable);
	m_password.EnableWindow(!enable);
}
