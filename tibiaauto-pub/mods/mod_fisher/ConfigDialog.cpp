// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_fisher.h"
#include "ConfigDialog.h"
#include <MemReader.h>
#include <TibiaItem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_fisherApp *app, CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_FRAME_STATISTICS, m_StatisticsFrame);
	DDX_Control(pDX, IDC_FRAME_OPTIONS, m_OptionsFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FISH_CAP, m_fishOnlyWhenCap);
	DDX_Control(pDX, IDC_AUTOFISH_FISHYWATER, m_fishyWater);
	DDX_Control(pDX, IDC_AUTOFISH_TIME, m_time);
	DDX_Control(pDX, IDC_AUTOFISH_QTY, m_quantity);
	DDX_Control(pDX, IDC_AUTOFISH_FISH_WHEN_WORMS, m_fishOnlyWhenWorms);
	DDX_Control(pDX, IDC_AUTOFISH_MOVETOCONT, m_moveFromHandToCont);
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
	m_fishOnlyWhenCap.EnableWindow(false);
	m_moveFromHandToCont.EnableWindow(false);
	m_fishOnlyWhenWorms.EnableWindow(false);
}

void CConfigDialog::enableControls()
{
	m_fishOnlyWhenCap.EnableWindow(true);
	m_moveFromHandToCont.EnableWindow(true);
	m_fishOnlyWhenWorms.EnableWindow(true);
}

void CConfigDialog::configToControls(CConfigData *configData)
{
	char buf[128];
	sprintf(buf, "%d", configData->fishOnlyWhenCap);
	m_fishOnlyWhenCap.SetWindowText(buf);
	m_moveFromHandToCont.SetCheck(configData->moveFromHandToCont);
	m_fishOnlyWhenWorms.SetCheck(configData->fishOnlyWhenWorms);
}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	m_fishOnlyWhenCap.GetWindowText(buf, 127);
	newConfigData->fishOnlyWhenCap    = atoi(buf);
	newConfigData->moveFromHandToCont = m_moveFromHandToCont.GetCheck();
	newConfigData->fishOnlyWhenWorms  = m_fishOnlyWhenWorms.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1001)
	{
		KillTimer(1001);

		refreshFishStatus();

		SetTimer(1001, 250, NULL);
	}

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

	refreshFishStatus();

	SetTimer(1001, 250, NULL);

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

void CConfigDialog::refreshFishStatus()
{
	CMemReader& reader = CMemReader::getMemReader();
	
	

	int totalFishQty = 0;

	int contNr;
	int openContNr  = 0;
	int openContMax = reader.readOpenContainerCount();
	for (contNr = 0; contNr < reader.m_memMaxContainers && openContNr < openContMax; contNr++)
	{
		CTibiaContainer *container = reader.readContainer(contNr);

		if (container->flagOnOff)
		{
			openContNr++;
			totalFishQty += container->countItemsOfType(CTibiaItem::getValueForConst("fish"));
		}

		delete container;
	}

	int fishTimeH   = totalFishQty * 144 / 3600;
	int fishTimeMin = (totalFishQty * 144 / 60) % 60;
	int fishTimeSec = totalFishQty * 144 % 60;

	char buf[256];
	sprintf(buf, "%d", totalFishQty);
	m_quantity.SetWindowText(buf);
	sprintf(buf, "%dh %dmin %ds", fishTimeH, fishTimeMin, fishTimeSec);
	m_time.SetWindowText(buf);

	// now count fishy water fields
	int x, y, fishyWaterCount;
	fishyWaterCount = 0;
	for (x = -7; x <= 7; x++)
	{
		for (y = -5; y <= 5; y++)
		{
			int tileId = reader.mapGetPointItemId(point(x, y, 0), 0);
			if (tileId >= CTibiaItem::getValueForConst("waterWithFishStart") && tileId <= CTibiaItem::getValueForConst("waterWithFishEnd"))
				fishyWaterCount++;
		}
	}
	sprintf(buf, "%d", fishyWaterCount);
	m_fishyWater.SetWindowText(buf);
}
