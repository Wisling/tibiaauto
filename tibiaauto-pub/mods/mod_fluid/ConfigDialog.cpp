// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_fluid.h"
#include "ConfigDialog.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CMod_fluidApp *app,CWnd* pParent /*=NULL*/)
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
	DDX_Control(pDX, IDC_FRAME_GENERAL_OPTIONS, m_GeneralOptionsFrame);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_MANA_RECOVERY, m_ManaRecoveryFrame);
	DDX_Control(pDX, IDC_FRAME_HEALTH_RECOVERY, m_HealthRecoveryFrame);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_MANABELOW_S, m_manaBelowS);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_MANABELOW_N, m_manaBelowN);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_MANABELOW_G, m_manaBelowG);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HPBELOW_H, m_hpBelowH);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HPBELOW_S, m_hpBelowS);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HPBELOW_N, m_hpBelowN);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HPBELOW_G, m_hpBelowG);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HPBELOW_U, m_hpBelowU);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKMANA_S, m_drinkManaS);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKMANA_N, m_drinkManaN);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKMANA_G, m_drinkManaG);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKHP_H, m_drinkHpH);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKHP_S, m_drinkHpS);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKHP_N, m_drinkHpN);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKHP_G, m_drinkHpG);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKHP_U, m_drinkHpU);
	DDX_Control(pDX, IDC_CUSTOM_ITEM_2_USE, m_customItem2Use);
	DDX_Control(pDX, IDC_CUSTOM_ITEM_2_BELOW, m_customItem2Below);
	DDX_Control(pDX, IDC_CUSTOM_ITEM_2, m_customItem2List);
	DDX_Control(pDX, IDC_CUSTOM_ITEM_1_USE, m_customItem1Use);
	DDX_Control(pDX, IDC_CUSTOM_ITEM_1_BELOW, m_customItem1Below);
	DDX_Control(pDX, IDC_CUSTOM_ITEM_1, m_customItem1List);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DROPEMPTY, m_dropEmpty);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_SLEEP, m_sleep);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_MANABELOW, m_manaBelow);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_HPBELOW, m_hpBelow);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKMANA, m_drinkMana);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_DRINKHP, m_drinkHp);
	DDX_Control(pDX, IDC_TOOL_RANDOMCAST,m_randomCast);
	DDX_Control(pDX, IDC_TOOL_USEHOTKEY,m_useHotkey);
	DDX_Control(pDX, IDC_ENABLE, m_enable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
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
	m_dropEmpty.EnableWindow(false);
	m_sleep.EnableWindow(false);
	m_manaBelow.EnableWindow(false);
	m_manaBelowN.EnableWindow(false);
	m_manaBelowS.EnableWindow(false);
	m_manaBelowG.EnableWindow(false);
	m_hpBelowH.EnableWindow(false);
	m_hpBelow.EnableWindow(false);
	m_hpBelowN.EnableWindow(false);
	m_hpBelowS.EnableWindow(false);
	m_hpBelowG.EnableWindow(false);
	m_hpBelowU.EnableWindow(false);
	m_drinkMana.EnableWindow(false);
	m_drinkManaN.EnableWindow(false);
	m_drinkManaS.EnableWindow(false);
	m_drinkManaG.EnableWindow(false);
	m_drinkHpH.EnableWindow(false);
	m_drinkHp.EnableWindow(false);
	m_drinkHpN.EnableWindow(false);
	m_drinkHpS.EnableWindow(false);
	m_drinkHpG.EnableWindow(false);
	m_drinkHpU.EnableWindow(false);
	m_customItem1Below.EnableWindow(false);
	m_customItem1List.EnableWindow(false);
	m_customItem1Use.EnableWindow(false);
	m_customItem2Below.EnableWindow(false);
	m_customItem2List.EnableWindow(false);
	m_customItem2Use.EnableWindow(false);
	m_randomCast.EnableWindow(false);
	m_useHotkey.EnableWindow(false);

}

void CConfigDialog::enableControls()
{
	m_dropEmpty.EnableWindow(true);
	m_sleep.EnableWindow(true);
	m_manaBelow.EnableWindow(true);
	m_manaBelowN.EnableWindow(true);
	m_manaBelowS.EnableWindow(true);
	m_manaBelowG.EnableWindow(true);
	m_hpBelowH.EnableWindow(true);
	m_hpBelow.EnableWindow(true);
	m_hpBelowN.EnableWindow(true);
	m_hpBelowS.EnableWindow(true);
	m_hpBelowG.EnableWindow(true);
	m_hpBelowU.EnableWindow(true);
	m_drinkMana.EnableWindow(true);
	m_drinkManaN.EnableWindow(true);
	m_drinkManaS.EnableWindow(true);
	m_drinkManaG.EnableWindow(true);
	m_drinkHpH.EnableWindow(true);
	m_drinkHp.EnableWindow(true);
	m_drinkHpN.EnableWindow(true);
	m_drinkHpS.EnableWindow(true);
	m_drinkHpG.EnableWindow(true);
	m_drinkHpU.EnableWindow(true);
	m_customItem1Below.EnableWindow(true);
	m_customItem1List.EnableWindow(true);
	m_customItem1Use.EnableWindow(true);
	m_customItem2Below.EnableWindow(true);
	m_customItem2List.EnableWindow(true);
	m_customItem2Use.EnableWindow(true);
	m_randomCast.EnableWindow(true);
	m_useHotkey.EnableWindow(true);
}



void CConfigDialog::configToControls(CConfigData *configData)
{
	CTibiaItemProxy itemProxy;
	char buf[128];

	m_dropEmpty.SetCheck(configData->dropEmpty);
	sprintf(buf,"%d",configData->sleep);m_sleep.SetWindowText(buf);
	sprintf(buf,"%d",configData->manaBelow);m_manaBelow.SetWindowText(buf);
	sprintf(buf,"%d",configData->manaBelowN);m_manaBelowN.SetWindowText(buf);
	sprintf(buf,"%d",configData->manaBelowS);m_manaBelowS.SetWindowText(buf);
	sprintf(buf,"%d",configData->manaBelowG);m_manaBelowG.SetWindowText(buf);
	sprintf(buf,"%d",configData->hpBelowH);m_hpBelowH.SetWindowText(buf);
	sprintf(buf,"%d",configData->hpBelow);m_hpBelow.SetWindowText(buf);
	sprintf(buf,"%d",configData->hpBelowN);m_hpBelowN.SetWindowText(buf);
	sprintf(buf,"%d",configData->hpBelowS);m_hpBelowS.SetWindowText(buf);
	sprintf(buf,"%d",configData->hpBelowG);m_hpBelowG.SetWindowText(buf);
	sprintf(buf,"%d",configData->hpBelowU);m_hpBelowU.SetWindowText(buf);
	m_drinkMana.SetCheck(configData->drinkMana);
	m_drinkManaN.SetCheck(configData->drinkManaN);
	m_drinkManaS.SetCheck(configData->drinkManaS);
	m_drinkManaG.SetCheck(configData->drinkManaG);
	m_drinkHpH.SetCheck(configData->drinkHpH);
	m_drinkHp.SetCheck(configData->drinkHp);
	m_drinkHpN.SetCheck(configData->drinkHpN);
	m_drinkHpS.SetCheck(configData->drinkHpS);
	m_drinkHpG.SetCheck(configData->drinkHpG);
	m_drinkHpU.SetCheck(configData->drinkHpU);

	m_customItem1Use.SetCheck(configData->customItem1Use);
	sprintf(buf,"%d",configData->customItem1Below);m_customItem1Below.SetWindowText(buf);
	m_customItem1List.SetCurSel(m_customItem1List.FindString(-1,itemProxy.getItemName(configData->customItem1Item)));
	if (m_customItem1List.GetCurSel()==-1) m_customItem1List.SetCurSel(0);

	m_customItem2Use.SetCheck(configData->customItem2Use);
	sprintf(buf,"%d",configData->customItem2Below);m_customItem2Below.SetWindowText(buf);
	m_customItem2List.SetCurSel(m_customItem2List.FindString(-1,itemProxy.getItemName(configData->customItem2Item)));
	if (m_customItem2List.GetCurSel()==-1) m_customItem2List.SetCurSel(0);

	m_randomCast.SetCheck(configData->randomCast);
	m_useHotkey.SetCheck(configData->useHotkey);

}

CConfigData * CConfigDialog::controlsToConfig()
{
	char buf[128];
	CConfigData *newConfigData = new CConfigData();

	newConfigData->dropEmpty=m_dropEmpty.GetCheck();
	m_sleep.GetWindowText(buf,127);newConfigData->sleep=atoi(buf);
	m_manaBelow.GetWindowText(buf,127);newConfigData->manaBelow=atoi(buf);
	m_manaBelowN.GetWindowText(buf,127);newConfigData->manaBelowN=atoi(buf);
	m_manaBelowS.GetWindowText(buf,127);newConfigData->manaBelowS=atoi(buf);
	m_manaBelowG.GetWindowText(buf,127);newConfigData->manaBelowG=atoi(buf);
	m_hpBelowH.GetWindowText(buf,127);newConfigData->hpBelowH=atoi(buf);
	m_hpBelow.GetWindowText(buf,127);newConfigData->hpBelow=atoi(buf);
	m_hpBelowN.GetWindowText(buf,127);newConfigData->hpBelowN=atoi(buf);
	m_hpBelowS.GetWindowText(buf,127);newConfigData->hpBelowS=atoi(buf);
	m_hpBelowG.GetWindowText(buf,127);newConfigData->hpBelowG=atoi(buf);
	m_hpBelowU.GetWindowText(buf,127);newConfigData->hpBelowU=atoi(buf);
	newConfigData->drinkMana=m_drinkMana.GetCheck();
	newConfigData->drinkManaN=m_drinkManaN.GetCheck();
	newConfigData->drinkManaS=m_drinkManaS.GetCheck();
	newConfigData->drinkManaG=m_drinkManaG.GetCheck();
	newConfigData->drinkHpH=m_drinkHpH.GetCheck();
	newConfigData->drinkHp=m_drinkHp.GetCheck();
	newConfigData->drinkHpN=m_drinkHpN.GetCheck();
	newConfigData->drinkHpS=m_drinkHpS.GetCheck();
	newConfigData->drinkHpG=m_drinkHpG.GetCheck();
	newConfigData->drinkHpU=m_drinkHpU.GetCheck();

	newConfigData->customItem1Use=m_customItem1Use.GetCheck();
	m_customItem1Below.GetWindowText(buf,127);newConfigData->customItem1Below=atoi(buf);
	newConfigData->customItem1Item=m_customItem1List.GetItemData(m_customItem1List.GetCurSel());

	newConfigData->customItem2Use=m_customItem2Use.GetCheck();
	m_customItem2Below.GetWindowText(buf,127);newConfigData->customItem2Below=atoi(buf);
	newConfigData->customItem2Item=m_customItem2List.GetItemData(m_customItem2List.GetCurSel());

	newConfigData->randomCast=m_randomCast.GetCheck();
	newConfigData->useHotkey=m_useHotkey.GetCheck();

	return newConfigData;
}

void CConfigDialog::OnTimer(UINT nIDEvent)
{
	
	CDialog::OnTimer(nIDEvent);
}

void CConfigDialog::DoSetButtonSkin(){
	skin.SetButtonSkin(	m_OK);
	skin.SetButtonSkin(	m_enable);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	reloadCustomItems();
	
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

void CConfigDialog::reloadCustomItems()
{
	CTibiaItemProxy itemProxy;

	while (m_customItem1List.GetCount()>0) m_customItem1List.DeleteString(0);
	while (m_customItem1List.GetCount()>0) m_customItem2List.DeleteString(0);

	// load items for depot item combo

	int size = itemProxy.getItemCount();
	for (int i=0;i<size;i++)
	{
		char* s=itemProxy.getItemNameAtIndex(i);
		int objectId=itemProxy.getItemIdAtIndex(i);
		int idx1 = m_customItem1List.AddString(s);
		int idx2 = m_customItem2List.AddString(s);
		m_customItem1List.SetItemData(idx1,objectId);
		m_customItem2List.SetItemData(idx2,objectId);
	}
	m_customItem1List.SetCurSel(0);
	m_customItem2List.SetCurSel(0);
}
