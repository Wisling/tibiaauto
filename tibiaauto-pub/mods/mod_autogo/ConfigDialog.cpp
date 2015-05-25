// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "ConfigDialog.h"
#include "WhiteList.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include <cmath>
#include "AlarmDialog.h"
#include "GeneralConfigDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

const UINT RWM_PRETRANSLATEMSG = ::RegisterWindowMessage(_T("RWM_PRETRANSLATEMSG"));

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_autogoApp *app, CWnd* pParent /*=NULL*/)
	: MyDialog(CConfigDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CConfigDialog)
	//}}AFX_DATA_INIT
	m_app         = app;
	statusPrinted = NULL;
	memset(memWhiteList, 0, 3200);
	m_DialogID[0] = IDD_ALARM_DIALOG;
	m_DialogID[1] = IDD_GENERAL_CONFIG_DIALOG;

	m_Dialog[0]  = new CAlarmDialog;
	m_Dialog[1]  = new GeneralConfigDialog;
	m_nPageCount = 2;
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Control(pDX, IDC_TAB_CONTROL, m_tabCtrl);
	DDX_Control(pDX, IDC_AUTOGO_STATUS, m_status);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDOK, m_OK);
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
ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers

void CConfigDialog::OnOK() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnClose() {
	ShowWindow(SW_HIDE);
}

void CConfigDialog::OnEnable() {
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

void CConfigDialog::disableControls() {
	m_Dialog[0]->disableControls();
	m_Dialog[1]->disableControls();
}

void CConfigDialog::enableControls() {
	m_Dialog[0]->enableControls();
	m_Dialog[1]->enableControls();
}

void CConfigDialog::configToControls(CConfigData *configData) {
	CTibiaItemProxy itemProxy;
	m_Dialog[0]->configToControls(configData);
	m_Dialog[1]->configToControls(configData);
	statusPrinted = configData->status;
}

CConfigData * CConfigDialog::controlsToConfig() {
	CTibiaItemProxy itemProxy;
	CConfigData *newConfigData = new CConfigData();
	m_Dialog[0]->controlsToConfig(newConfigData);
	m_Dialog[1]->controlsToConfig(newConfigData);
	statusPrinted = newConfigData->status;
	return newConfigData;
}

static int lastX = 0, lastY = 0, lastZ = 0;

void CConfigDialog::OnTimer(UINT nIDEvent) {
	if (nIDEvent == 1001)
	{
		CMemReaderProxy reader;
		CTibiaCharacter *self = reader.readSelfCharacter();
		char buf[512];

		if (!m_enable.GetCheck())
			triggerMessage();

		sprintf(buf, "Status: %s", statusPrinted);
		m_status.SetWindowText(buf);

		delete self;
	}
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(     m_enable);
	skin.SetButtonSkin(     m_OK);
	m_tabCtrl.SetTabColor(RGB(skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue));
	m_tabCtrl.SetNormalColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
	((CAlarmDialog*) m_Dialog[0])->DoSetButtonSkin();
	((GeneralConfigDialog*) m_Dialog[1])->DoSetButtonSkin();
}

BOOL CConfigDialog::OnInitDialog() {
	CDialog::OnInitDialog();

	m_Dialog[0]->Create(m_DialogID[0], &m_tabCtrl);
	m_Dialog[1]->Create(m_DialogID[1], &m_tabCtrl);

	CRect rect;
	m_tabCtrl.GetClientRect(rect);
	m_tabCtrl.SetMinTabWidth(((rect.right - rect.left) / m_nPageCount) - 2);
	m_tabCtrl.SetTopLeftCorner(CPoint(3, 22));

	m_tabCtrl.AddTab(m_Dialog[0], "Alarms");
	m_tabCtrl.AddTab(m_Dialog[1], "General Configuration");

	DoSetButtonSkin();
	SetTimer(1001, 250, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg) {
	return MyDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable) {
	m_enable.SetCheck(enable);
}

void CConfigDialog::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: Add your message handler code here

	CDialog::OnCaptureChanged(pWnd);
}

void CConfigDialog::OnDestroy() {
	delete(m_Dialog[0]);
	delete(m_Dialog[1]);
	MyDialog::OnDestroy();
}


LRESULT CConfigDialog::WindowProc(UINT msg, WPARAM wp, LPARAM lp){
	if(msg == RWM_PRETRANSLATEMSG)
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
