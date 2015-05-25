// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_fps.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(Cmod_fpsApp *app, CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_OPTIONS, m_OptionsFrame);
	DDX_Control(pDX, IDC_FPSTOOL_TIBIASTATE, m_state);
	DDX_Control(pDX, IDC_FPSTOOL_MINIMIZEDVAL, m_minimizedVal);
	DDX_Control(pDX, IDC_FPSTOOL_MINIMIZED, m_minimized);
	DDX_Control(pDX, IDC_FPSTOOL_INACTIVEVAL, m_inactiveVal);
	DDX_Control(pDX, IDC_FPSTOOL_INACTIVE, m_inactive);
	DDX_Control(pDX, IDC_FPSTOOL_FPSRATE, m_fpsRate);
	DDX_Control(pDX, IDC_FPSTOOL_ACTIVEVAL, m_activeVal);
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
ON_BN_CLICKED(IDC_FPSTOOL_INACTIVE, OnFpstoolInactive)
ON_BN_CLICKED(IDC_FPSTOOL_MINIMIZED, OnFpstoolMinimized)
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
	m_activeVal.EnableWindow(false);
	m_inactive.EnableWindow(false);
	m_inactiveVal.EnableWindow(false);
	m_minimized.EnableWindow(false);
	m_minimizedVal.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_activeVal.EnableWindow(true);
	m_inactive.EnableWindow(true);
	if (m_inactive.GetCheck())
		m_inactiveVal.EnableWindow(true);
	m_minimized.EnableWindow(true);
	if (m_minimized.GetCheck())
		m_minimizedVal.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	char buff[128];
	sprintf(buff, "%.2f", configData->activeVal);
	m_activeVal.SetWindowText(buff);
	m_inactive.SetCheck(configData->inactive);
	sprintf(buff, "%.2f", configData->inactiveVal);
	m_inactiveVal.SetWindowText(buff);
	m_minimized.SetCheck(configData->minimized);
	sprintf(buff, "%.2f", configData->minimizedVal);
	m_minimizedVal.SetWindowText(buff);

	OnFpstoolInactive();
	OnFpstoolMinimized();
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CConfigData *newConfigData = new CConfigData();

	char buff[128];
	m_activeVal.GetWindowText(buff, 128);
	newConfigData->activeVal = atof(buff);
	newConfigData->inactive  = m_inactive.GetCheck();
	m_inactiveVal.GetWindowText(buff, 128);
	newConfigData->inactiveVal = atof(buff);
	newConfigData->minimized   = m_minimized.GetCheck();
	m_minimizedVal.GetWindowText(buff, 128);
	newConfigData->minimizedVal = atof(buff);

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1000)
	{
		CMemReaderProxy reader;
		CTibiaItemProxy itemProxy;
		//CPackSenderProxy sender;

		char buf[128];
		unsigned long ifpsCur[2];

		addrFps = reader.getMemIntValue(itemProxy.getValueForConst("addrFps"));

		ifpsCur[0] = reader.getMemIntValue(addrFps + 0x60, 0);//this address comes from Tibia itself and need not be shifted
		ifpsCur[1] = reader.getMemIntValue(addrFps + 0x60 + 4, 0);//this address comes from Tibia itself and need not be shifted
		sprintf(buf, "Current FPS rate: %f", ifpsCur[0], ifpsCur[1]);
		m_fpsRate.SetWindowText(buf);

		if (hTibiaWnd)
		{
			if (::IsIconic(hTibiaWnd))
			{
				m_state.SetWindowText("Tibia state: Minimized");
			}
			else
			{
				if (::GetForegroundWindow() != hTibiaWnd)
					m_state.SetWindowText("Tibia state: Inactive");
				else
					m_state.SetWindowText("Tibia state: Active");
			}
		}
		else
		{
			m_state.SetWindowText("Tibia state: Unknown");
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD procId;
	GetWindowThreadProcessId(hwnd, &procId);
	if (procId == (DWORD)lParam)
	{
		char szClass[128];
		::GetClassName(hwnd, szClass, 128);
		if (lstrcmp(szClass, "TibiaClient") == 0)
		{
			hTibiaWnd = hwnd;
			return false;
		}
	}
	return true;
}

void CConfigDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_enable);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;

	addrFps = reader.getMemIntValue(itemProxy.getValueForConst("addrFps"));

	EnumWindows(EnumWindowsProc, reader.getProcessId());

	SetTimer(1000, 500, NULL);

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

void CConfigDialog::OnFpstoolInactive()
{
	int val = m_inactive.GetCheck();
	m_inactiveVal.EnableWindow(val);
}

void CConfigDialog::OnFpstoolMinimized()
{
	int val = m_minimized.GetCheck();
	m_minimizedVal.EnableWindow(val);
}
