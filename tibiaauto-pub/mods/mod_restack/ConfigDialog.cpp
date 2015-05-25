// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_restack.h"
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


CConfigDialog::CConfigDialog(CMod_restackApp *app, CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_FRAME_THROWABLES, m_ThrowablesFrame);
	DDX_Control(pDX, IDC_FRAME_PICKUP_AREA, m_PickupAreaFrame);
	DDX_Control(pDX, IDC_FRAME_OPTIONS, m_OptionsFrame);
	DDX_Control(pDX, IDC_FRAME_AMMUNITION, m_AmmunitionFrame);
	DDX_Control(pDX, IDC_CAP_LIMIT, m_capLimit);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_CC, m_pickupCC);
	DDX_Control(pDX, IDC_PERIOD_TO, m_periodTo);
	DDX_Control(pDX, IDC_PERIOD_FROM, m_periodFrom);
	DDX_Control(pDX, IDC_PICKUP_TO_HAND, m_pickupToHand);
	DDX_Control(pDX, IDC_TOOL_RESTACKTORIGHT, m_restackToRight);
	DDX_Control(pDX, IDC_TOOL_ARMMORESTACK_PICKUPCOVERED, m_moveCovering);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_UR, m_pickupUR);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_UL, m_pickupUL);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_UC, m_pickupUC);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_CR, m_pickupCR);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_CL, m_pickupCL);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_BR, m_pickupBR);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_BL, m_pickupBL);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_PICKUP_BC, m_pickupBC);
	DDX_Control(pDX, IDC_TOOL_ARMMORESTACK_PICKUP, m_pickupSpears);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_THROWABLE_TO, m_throwableTo);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_THROWABLE_AT, m_throwableAt);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_THROWABLE, m_throwableType);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_AMMO_TO, m_ammoTo);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_AMMO_AT, m_ammoAt);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_AMMO, m_ammoType);
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
	m_pickupUR.EnableWindow(false);
	m_pickupUL.EnableWindow(false);
	m_pickupUC.EnableWindow(false);
	m_pickupCR.EnableWindow(false);
	m_pickupCC.EnableWindow(false);
	m_pickupCL.EnableWindow(false);
	m_pickupBR.EnableWindow(false);
	m_pickupBL.EnableWindow(false);
	m_pickupBC.EnableWindow(false);
	m_pickupSpears.EnableWindow(false);
	m_throwableTo.EnableWindow(false);
	m_throwableAt.EnableWindow(false);
	m_throwableType.EnableWindow(false);
	m_ammoTo.EnableWindow(false);
	m_ammoAt.EnableWindow(false);
	m_ammoType.EnableWindow(false);
	m_moveCovering.EnableWindow(false);
	m_restackToRight.EnableWindow(false);
	m_pickupToHand.EnableWindow(false);
	m_periodFrom.EnableWindow(false);
	m_periodTo.EnableWindow(false);
	m_capLimit.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_pickupUR.EnableWindow(true);
	m_pickupUL.EnableWindow(true);
	m_pickupUC.EnableWindow(true);
	m_pickupCR.EnableWindow(true);
	m_pickupCC.EnableWindow(true);
	m_pickupCL.EnableWindow(true);
	m_pickupBR.EnableWindow(true);
	m_pickupBL.EnableWindow(true);
	m_pickupBC.EnableWindow(true);
	m_pickupSpears.EnableWindow(true);
	m_throwableTo.EnableWindow(true);
	m_throwableAt.EnableWindow(true);
	m_throwableType.EnableWindow(true);
	m_ammoTo.EnableWindow(true);
	m_ammoAt.EnableWindow(true);
	m_ammoType.EnableWindow(true);
	m_moveCovering.EnableWindow(true);
	m_restackToRight.EnableWindow(true);
	m_pickupToHand.EnableWindow(true);
	m_periodFrom.EnableWindow(true);
	m_periodTo.EnableWindow(true);
	m_capLimit.EnableWindow(true);
}


void CConfigDialog::configToControls(CConfigData *configData)
{
	CTibiaItemProxy itemProxy;
	char buf[128];
	m_pickupUR.SetCheck(configData->pickupUR);
	m_pickupUL.SetCheck(configData->pickupUL);
	m_pickupUC.SetCheck(configData->pickupUC);
	m_pickupCR.SetCheck(configData->pickupCR);
	m_pickupCC.SetCheck(configData->pickupCC);
	m_pickupCL.SetCheck(configData->pickupCL);
	m_pickupBR.SetCheck(configData->pickupBR);
	m_pickupBL.SetCheck(configData->pickupBL);
	m_pickupBC.SetCheck(configData->pickupBC);
	m_pickupSpears.SetCheck(configData->pickupSpears);
	sprintf(buf, "%d", configData->throwableTo); m_throwableTo.SetWindowText(buf);
	sprintf(buf, "%d", configData->throwableAt); m_throwableAt.SetWindowText(buf);
	m_throwableType.SelectString(-1, itemProxy.getItemName(configData->throwableType));
	sprintf(buf, "%d", configData->ammoTo); m_ammoTo.SetWindowText(buf);
	sprintf(buf, "%d", configData->ammoAt); m_ammoAt.SetWindowText(buf);
	m_ammoType.SelectString(-1, itemProxy.getItemName(configData->ammoType));
	m_moveCovering.SetCheck(configData->moveCovering);
	m_restackToRight.SetCheck(configData->restackToRight);
	m_pickupToHand.SetCheck(configData->pickupToHand);
	sprintf(buf, "%d", configData->periodFrom); m_periodFrom.SetWindowText(buf);
	sprintf(buf, "%d", configData->periodTo); m_periodTo.SetWindowText(buf);
	sprintf(buf, "%d", configData->capLimit); m_capLimit.SetWindowText(buf);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	CTibiaItemProxy itemProxy;
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	newConfigData->pickupUR                                           = m_pickupUR.GetCheck();
	newConfigData->pickupUL                                           = m_pickupUL.GetCheck();
	newConfigData->pickupUC                                           = m_pickupUC.GetCheck();
	newConfigData->pickupCR                                           = m_pickupCR.GetCheck();
	newConfigData->pickupCC                                           = m_pickupCC.GetCheck();
	newConfigData->pickupCL                                           = m_pickupCL.GetCheck();
	newConfigData->pickupBR                                           = m_pickupBR.GetCheck();
	newConfigData->pickupBL                                           = m_pickupBL.GetCheck();
	newConfigData->pickupBC                                           = m_pickupBC.GetCheck();
	newConfigData->pickupSpears                                       = m_pickupSpears.GetCheck();
	m_throwableTo.GetWindowText(buf, 127); newConfigData->throwableTo = atoi(buf);
	m_throwableAt.GetWindowText(buf, 127); newConfigData->throwableAt = atoi(buf);

	buf[0] = '\0';
	m_throwableType.GetLBText(m_throwableType.GetCurSel(), buf);
	newConfigData->throwableType = itemProxy.getItemId(buf);

	m_ammoTo.GetWindowText(buf, 127); newConfigData->ammoTo = atoi(buf);
	m_ammoAt.GetWindowText(buf, 127); newConfigData->ammoAt = atoi(buf);
	buf[0]                                                  = '\0';
	m_ammoType.GetLBText(m_ammoType.GetCurSel(), buf);
	newConfigData->ammoType = itemProxy.getItemId(buf);


	newConfigData->moveCovering                                     = m_moveCovering.GetCheck();
	newConfigData->restackToRight                                   = m_restackToRight.GetCheck();
	newConfigData->pickupToHand                                     = m_pickupToHand.GetCheck();
	m_periodFrom.GetWindowText(buf, 127); newConfigData->periodFrom = atoi(buf);
	m_periodTo.GetWindowText(buf, 127); newConfigData->periodTo     = atoi(buf);
	m_capLimit.GetWindowText(buf, 127); newConfigData->capLimit     = atoi(buf);


	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(     m_enable);
	skin.SetButtonSkin(     m_OK);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	// reload ammo and throwable combo boxes
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;

	while (m_ammoType.GetCount() > 0)
		m_ammoType.DeleteString(0);
	while (m_throwableType.GetCount() > 0)
		m_throwableType.DeleteString(0);

	// load items for depot item combo
	int count = itemProxy.getItemCount();
	int i;
	m_ammoType.AddString("<disabled>");
	m_throwableType.AddString("<disabled>");

	for (i = 0; i < count; i++)
	{
		char* name       = itemProxy.getItemNameAtIndex(i);
		int objectId     = itemProxy.getItemIdAtIndex(i);
		CTibiaTile *tile = reader.getTibiaTile(objectId);
		if (tile && tile->stackable)
		{
			m_ammoType.AddString(name);
			m_throwableType.AddString(name);
		}
	}
	m_ammoType.SetCurSel(0);
	m_throwableType.SetCurSel(0);


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
