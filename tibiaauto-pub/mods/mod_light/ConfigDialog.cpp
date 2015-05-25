// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_light.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(Cmod_lightApp *app, CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_FRAME_LIGHT_DEFINITION, m_LightDefinitionFrame);
	DDX_Control(pDX, IDC_TOOLLIGHT_COMBO, m_combo);
	DDX_Control(pDX, IDC_TOOLLIGHT_POWER, m_power);
	DDX_Control(pDX, IDC_TOOLLIGHT_COLOR, m_color);
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
ON_CBN_SELCHANGE(IDC_TOOLLIGHT_COMBO, OnSelchangeToollightCombo)
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
	m_combo.EnableWindow(false);
	m_power.EnableWindow(false);
	m_color.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_combo.EnableWindow(true);
	if (m_combo.GetItemData(m_combo.GetCurSel()) == 0xFFFFFFFF)
	{
		m_power.EnableWindow(true);
		m_color.EnableWindow(true);
	}
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	char buff[128];
	sprintf(buff, "%d", configData->lightPower);
	m_power.SetWindowText(buff);
	sprintf(buff, "%d", configData->lightColor);
	m_color.SetWindowText(buff);

	DWORD dwData = MAKEWPARAM(configData->lightPower, configData->lightColor);

	for (int i = 0; i < m_combo.GetCount(); i++)
	{
		if (dwData == m_combo.GetItemData(i))
		{
			m_combo.SetCurSel(i);
			break;
		}
		else
		{
			m_combo.SetCurSel(4);
		}
	}
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CConfigData *newConfigData = new CConfigData();

	char buff[128];
	m_power.GetWindowText(buff, 128);
	newConfigData->lightPower = atoi(buff);
	m_color.GetWindowText(buff, 128);
	newConfigData->lightColor = atoi(buff);

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
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

	m_combo.InsertString(0, "LightHack");
	m_combo.SetItemData(0, MAKEWPARAM(100, 209));
	m_combo.InsertString(1, "Utevo Lux");
	m_combo.SetItemData(1, MAKEWPARAM(6, 215));
	m_combo.InsertString(2, "Torch");
	m_combo.SetItemData(2, MAKEWPARAM(7, 206));
	m_combo.InsertString(3, "Light Wand");
	m_combo.SetItemData(3, MAKEWPARAM(8, 209));
	m_combo.InsertString(4, "Custom...");
	m_combo.SetItemData(4, 0xFFFFFFFF);

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

void CConfigDialog::OnSelchangeToollightCombo()
{
	// TODO: Add your control notification handler code here
	int iCurSel = m_combo.GetCurSel();
	DWORD dwData;
	WORD power;
	WORD color;

	dwData = m_combo.GetItemData(iCurSel);
	if (dwData == 0xFFFFFFFF)
	{
		m_color.EnableWindow(true);
		m_power.EnableWindow(true);
	}
	else
	{
		color = WORD((dwData & 0xFFFF0000) >> 16);
		power = WORD(dwData & 0xFFFF);

		char buffer[128];
		sprintf(buffer, "%d", color);
		m_color.SetWindowText(buffer);
		m_color.EnableWindow(false);

		sprintf(buffer, "%d", power);
		m_power.SetWindowText(buffer);
		m_power.EnableWindow(false);
	}
}
