// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_looter.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_looterApp *app, CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_LOOT_DEPOT, m_lootInDepot);
	DDX_Control(pDX, IDC_FRAME_LOOT_OPTIONS, m_LootOptionsFrame);
	DDX_Control(pDX, IDC_FRAME_CONTAINER_SETUP, m_ContainerSetupFrame);
	DDX_Control(pDX, IDC_CONTAINER_STATUS10, m_status10);
	DDX_Control(pDX, IDC_CONTAINER_MODE_10, m_mode10);
	DDX_Control(pDX, IDC_LOOT_WORMS, m_lootWorms);
	DDX_Control(pDX, IDC_LOOT_GP, m_lootGp);
	DDX_Control(pDX, IDC_LOOT_FOOD, m_lootFood);
	DDX_Control(pDX, IDC_EAT_FROM_CORPSE, m_eatFromCorpse);
	DDX_Control(pDX, IDC_LOOT_CUSTOM, m_lootCustom);
	DDX_Control(pDX, IDC_AUTO_OPEN, m_autoOpen);
	DDX_Control(pDX, IDC_CONTAINER_STATUS9, m_status9);
	DDX_Control(pDX, IDC_CONTAINER_STATUS8, m_status8);
	DDX_Control(pDX, IDC_CONTAINER_STATUS7, m_status7);
	DDX_Control(pDX, IDC_CONTAINER_STATUS6, m_status6);
	DDX_Control(pDX, IDC_CONTAINER_STATUS5, m_status5);
	DDX_Control(pDX, IDC_CONTAINER_STATUS4, m_status4);
	DDX_Control(pDX, IDC_CONTAINER_STATUS3, m_status3);
	DDX_Control(pDX, IDC_CONTAINER_STATUS2, m_status2);
	DDX_Control(pDX, IDC_CONTAINER_STATUS1, m_status1);
	DDX_Control(pDX, IDC_CONTAINER_MODE_9, m_mode9);
	DDX_Control(pDX, IDC_CONTAINER_MODE_8, m_mode8);
	DDX_Control(pDX, IDC_CONTAINER_MODE_7, m_mode7);
	DDX_Control(pDX, IDC_CONTAINER_MODE_6, m_mode6);
	DDX_Control(pDX, IDC_CONTAINER_MODE_5, m_mode5);
	DDX_Control(pDX, IDC_CONTAINER_MODE_4, m_mode4);
	DDX_Control(pDX, IDC_CONTAINER_MODE_3, m_mode3);
	DDX_Control(pDX, IDC_CONTAINER_MODE_2, m_mode2);
	DDX_Control(pDX, IDC_CONTAINER_MODE_1, m_mode1);
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
	m_eatFromCorpse.EnableWindow(false);
	m_lootInDepot.EnableWindow(false);
	m_lootWorms.EnableWindow(false);
	m_lootGp.EnableWindow(false);
	m_lootFood.EnableWindow(false);
	m_lootCustom.EnableWindow(false);
	m_autoOpen.EnableWindow(false);
	m_mode10.EnableWindow(false);
	m_mode9.EnableWindow(false);
	m_mode8.EnableWindow(false);
	m_mode7.EnableWindow(false);
	m_mode6.EnableWindow(false);
	m_mode5.EnableWindow(false);
	m_mode4.EnableWindow(false);
	m_mode3.EnableWindow(false);
	m_mode2.EnableWindow(false);
	m_mode1.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_eatFromCorpse.EnableWindow(true);
	m_lootInDepot.EnableWindow(true);
	m_lootWorms.EnableWindow(true);
	m_lootGp.EnableWindow(true);
	m_lootFood.EnableWindow(true);
	m_lootCustom.EnableWindow(true);
	m_autoOpen.EnableWindow(true);
	m_mode10.EnableWindow(true);
	m_mode9.EnableWindow(true);
	m_mode8.EnableWindow(true);
	m_mode7.EnableWindow(true);
	m_mode6.EnableWindow(true);
	m_mode5.EnableWindow(true);
	m_mode4.EnableWindow(true);
	m_mode3.EnableWindow(true);
	m_mode2.EnableWindow(true);
	m_mode1.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	m_lootCustom.SetCheck(configData->m_lootCustom);
	m_lootGp.SetCheck(configData->m_lootGp);
	m_lootFood.SetCheck(configData->m_lootFood);
	m_eatFromCorpse.SetCheck(configData->m_eatFromCorpse);
	m_lootWorms.SetCheck(configData->m_lootWorms);
	m_lootInDepot.SetCheck(configData->m_lootInDepot);
	m_autoOpen.SetCheck(configData->m_autoOpen);
	m_mode1.SetCurSel(configData->m_mode1);
	m_mode2.SetCurSel(configData->m_mode2);
	m_mode3.SetCurSel(configData->m_mode3);
	m_mode4.SetCurSel(configData->m_mode4);
	m_mode5.SetCurSel(configData->m_mode5);
	m_mode6.SetCurSel(configData->m_mode6);
	m_mode7.SetCurSel(configData->m_mode7);
	m_mode8.SetCurSel(configData->m_mode8);
	m_mode9.SetCurSel(configData->m_mode9);
	m_mode10.SetCurSel(configData->m_mode10);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CConfigData *newConfigData = new CConfigData();

	newConfigData->m_lootCustom    = m_lootCustom.GetCheck();
	newConfigData->m_lootGp        = m_lootGp.GetCheck();
	newConfigData->m_lootFood      = m_lootFood.GetCheck();
	newConfigData->m_eatFromCorpse = m_eatFromCorpse.GetCheck();
	newConfigData->m_lootWorms     = m_lootWorms.GetCheck();
	newConfigData->m_lootInDepot   = m_lootInDepot.GetCheck();
	newConfigData->m_autoOpen      = m_autoOpen.GetCheck();
	newConfigData->m_mode1         = m_mode1.GetCurSel();
	newConfigData->m_mode2         = m_mode2.GetCurSel();
	newConfigData->m_mode3         = m_mode3.GetCurSel();
	newConfigData->m_mode4         = m_mode4.GetCurSel();
	newConfigData->m_mode5         = m_mode5.GetCurSel();
	newConfigData->m_mode6         = m_mode6.GetCurSel();
	newConfigData->m_mode7         = m_mode7.GetCurSel();
	newConfigData->m_mode8         = m_mode8.GetCurSel();
	newConfigData->m_mode9         = m_mode9.GetCurSel();
	newConfigData->m_mode10        = m_mode10.GetCurSel();


	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1001)
	{
		KillTimer(1001);

		refreshContainerStatus();

		SetTimer(1001, 250, NULL);
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

	refreshContainerStatus();

	SetTimer(1001, 250, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDialog::refreshContainerStatus()
{
	char buf[128];
	CMemReaderProxy reader;

	CTibiaContainer *cont;

	cont = reader.readContainer(0);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status1.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(1);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status2.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(2);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status3.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(3);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status4.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(4);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status5.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(5);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status6.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(6);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status7.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(7);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status8.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(8);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status9.SetWindowText(buf);
	delete cont;

	cont = reader.readContainer(9);
	sprintf(buf, "Status: %s", cont->flagOnOff ? "opened" : "closed");
	m_status10.SetWindowText(buf);
	delete cont;
}

BOOL CConfigDialog::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

void CConfigDialog::activateEnableButton(int enable)
{
	m_enable.SetCheck(enable);
}
