#include "stdafx.h"
#include "mod_spellcaster.h"
#include "ConfigDialog.h"
#include <MemReader.h>
#include <PackSender.h>
#include "LifeDialog.h"
#include "ManaDialog.h"
#include "SummonDialog.h"
#include "StrikeDialog.h"
#include "AOEDialog.h"
#include "TimedDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

const UINT RWM_PRETRANSLATEMSG = ::RegisterWindowMessage(_T("RWM_PRETRANSLATEMSG"));

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_spellcasterApp *app, CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app = app;

	m_DialogID[0] = IDD_LIFE_DIALOG;
	m_DialogID[1] = IDD_MANA_DIALOG;
	m_DialogID[2] = IDD_SUMMON_DIALOG;
	m_DialogID[3] = IDD_STRIKE_DIALOG;
	m_DialogID[4] = IDD_AOE_DIALOG;
	m_DialogID[5] = IDD_TIMED_DIALOG;

	m_Dialog[0] = new LifeDialog(this);
	m_Dialog[1] = new ManaDialog(this);
	m_Dialog[2] = new SummonDialog(this);
	m_Dialog[3] = new StrikeDialog(this);
	m_Dialog[4] = new AOEDialog(this);
	m_Dialog[5] = new TimedDialog(this);

	m_nPageCount = 6;
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_DISABLE_WARNING, m_disableWarning);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_RANDOM_CAST, m_randomCast);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDOK, m_OK);
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
	m_Dialog[0]->disableControls();
	m_Dialog[1]->disableControls();
	m_Dialog[2]->disableControls();
	m_Dialog[3]->disableControls();
	m_Dialog[4]->disableControls();
	m_Dialog[5]->disableControls();

	m_disableWarning.EnableWindow(false);
	m_randomCast.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_Dialog[0]->enableControls();
	m_Dialog[1]->enableControls();
	m_Dialog[2]->enableControls();
	m_Dialog[3]->enableControls();
	m_Dialog[4]->enableControls();
	m_Dialog[5]->enableControls();

	m_disableWarning.EnableWindow(true);
	m_randomCast.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	m_Dialog[0]->configToControls(configData);
	m_Dialog[1]->configToControls(configData);
	m_Dialog[2]->configToControls(configData);
	m_Dialog[3]->configToControls(configData);
	m_Dialog[4]->configToControls(configData);
	m_Dialog[5]->configToControls(configData);

	m_disableWarning.SetCheck(configData->disableWarning);
	m_randomCast.SetCheck(configData->randomCast);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CConfigData *newConfigData = new CConfigData();

	m_Dialog[0]->controlsToConfig(newConfigData);
	m_Dialog[1]->controlsToConfig(newConfigData);
	m_Dialog[2]->controlsToConfig(newConfigData);
	m_Dialog[3]->controlsToConfig(newConfigData);
	m_Dialog[4]->controlsToConfig(newConfigData);
	m_Dialog[5]->controlsToConfig(newConfigData);

	newConfigData->disableWarning = m_disableWarning.GetCheck();
	newConfigData->randomCast     = m_randomCast.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_enable);
	skin.SetButtonSkin(m_OK);
	((LifeDialog*)m_Dialog[0])->DoSetButtonSkin();
	((TimedDialog*)m_Dialog[5])->DoSetButtonSkin();
	m_tabCtrl.SetTabColor(RGB(skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue));
	m_tabCtrl.SetNormalColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Dialog[0]->Create(m_DialogID[0], &m_tabCtrl);
	m_Dialog[1]->Create(m_DialogID[1], &m_tabCtrl);
	m_Dialog[2]->Create(m_DialogID[2], &m_tabCtrl);
	m_Dialog[3]->Create(m_DialogID[3], &m_tabCtrl);
	m_Dialog[4]->Create(m_DialogID[4], &m_tabCtrl);
	m_Dialog[5]->Create(m_DialogID[5], &m_tabCtrl);
	DoSetButtonSkin();

	CPoint topLeft(3, 39);
	m_tabCtrl.SetTopLeftCorner(topLeft);

	m_tabCtrl.AddTab(m_Dialog[0], "Healing");
	m_tabCtrl.AddTab(m_Dialog[1], "Mana Burning");
	m_tabCtrl.AddTab(m_Dialog[2], "Summon Control");
	m_tabCtrl.AddTab(m_Dialog[3], "Strike Spells");
	m_tabCtrl.AddTab(m_Dialog[4], "AOE Spells");
	m_tabCtrl.AddTab(m_Dialog[5], "Timed Spells");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int pressed(unsigned int tm)
{
	char buf[1111];
	sprintf(buf, "tm %d", GetTickCount() - tm);
	CPackSender::sendTAMessage(buf);
	return GetTickCount() - tm < 300;
}

BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg)
{
	static int shift = 0, ctrl = 0, win = 0;
	LPARAM lParam    = pMsg->lParam;
	WPARAM wParam    = pMsg->wParam;
	int repeatCount  = lParam & 0xFFFF;
	int scanCode     = (lParam >> 16) & 0xFF;
	int extendedKey  = (lParam >> 24) & 0x1;
	int reserved     = (lParam >> 25) & 0xF;
	int contextCode  = (lParam >> 29) & 0x1;
	int keyState     = (lParam >> 30) & 0x1;
	int transState   = (lParam >> 31) & 0x1;
	int alt          = contextCode;

	char buf[1111];
	sprintf(buf, "rept %d,scan %d, xtend %d, rsvd %d, ctxt %d, keyst %d, transst %d, keyCode=%d", repeatCount, scanCode, extendedKey, reserved, contextCode, keyState, transState, wParam);
	CPackSender::sendTAMessage(buf);
	if (wParam == 0x10)
	{
		shift = GetTickCount();
		goto skip;
	}
	else if (wParam == 0x11)
	{
		ctrl = GetTickCount();
		goto skip;
	}
	else if (wParam == 0x12)
	{
		goto skip;
	}
	else if (wParam == 0x5B)
	{
		win = GetTickCount();
		goto skip;
	}
	if (pressed(ctrl) && wParam == 0x20)  // Ctrl+space Available
		return 1;
skip:
	return MyDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}

LRESULT CConfigDialog::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == RWM_PRETRANSLATEMSG)
	{
		MSG* pMsg = (MSG*)lp;
		ASSERT(pMsg);

		HWND hWndStop = this->GetSafeHwnd();
		ASSERT(hWndStop == NULL || ::IsWindow(hWndStop));

		//Loop through all windows from the receiver of the message to the highest ancestor
		for (HWND hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
		{
			CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
			if (pWnd != NULL)
				if (pWnd->PreTranslateMessage(pMsg))
					return TRUE;
			if (hWnd == hWndStop)
				break;
		}

		return FALSE; //do nothing
	}
	return MyDialog::WindowProc(msg, wp, lp);
}
